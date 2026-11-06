#include <tls/renderer.hpp>
#include <tls/shade_ai.hpp>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/packed_vector2_array.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <memory>
#include <mutex>
#include <atomic>
#include <cmath>
#include <algorithm>
using namespace godot;
static glm::vec2 vec(Vector2 p){return {float(p.x),float(p.y)};}
static Vector2 vec(glm::vec2 p){return {p.x,p.y};}
static std::vector<tls::Segment> segments(const PackedVector2Array& data){std::vector<tls::Segment> out;out.reserve(data.size()/2);for(int64_t i=0;i+1<data.size();i+=2) out.push_back({vec(data[i]),vec(data[i+1])});return out;}

/// Native occluder queries shared with the independent OpenGL renderer.
class TLSVisibility : public RefCounted {
    GDCLASS(TLSVisibility,RefCounted)
    std::vector<tls::Segment> walls;
protected:
    static void _bind_methods(){ClassDB::bind_method(D_METHOD("set_walls","edges"),&TLSVisibility::set_walls);ClassDB::bind_method(D_METHOD("line_of_sight","a","b"),&TLSVisibility::line_of_sight);ClassDB::bind_method(D_METHOD("polygon","origin","radius"),&TLSVisibility::polygon);}
public:
    void set_walls(PackedVector2Array edges){walls=segments(edges);}
    bool line_of_sight(Vector2 a,Vector2 b)const{return tls::Visibility::line_of_sight(vec(a),vec(b),walls);}
    PackedVector2Array polygon(Vector2 origin,double radius)const{std::vector<glm::vec2> p;tls::Visibility::polygon(vec(origin),walls,float(radius),p);PackedVector2Array out;for(auto v:p) out.push_back(vec(v));return out;}
};
/// Registered C++ finite-state enemy, exposing only frame inputs and results to scripts.
class TLSShadeAI : public RefCounted {
    GDCLASS(TLSShadeAI,RefCounted)
    tls::ShadeAI ai;std::vector<tls::Segment> walls;
protected:
    static void _bind_methods(){
        ClassDB::bind_method(D_METHOD("setup","spawn","waypoints","edges"),&TLSShadeAI::setup);
        ClassDB::bind_method(D_METHOD("tick","dt","player","flashlight","hidden","noise"),&TLSShadeAI::tick);
        ClassDB::bind_method(D_METHOD("get_position"),&TLSShadeAI::get_position);ClassDB::bind_method(D_METHOD("get_state"),&TLSShadeAI::get_state);
    }
public:
    void setup(Vector2 spawn,PackedVector2Array points,PackedVector2Array edges){ai=tls::ShadeAI();ai.position=ai.home=vec(spawn);for(auto p:points) ai.waypoints.push_back(vec(p));walls=segments(edges);}
    bool tick(double dt,Vector2 player,bool flashlight,bool hidden,double noise){return ai.update(float(dt),vec(player),flashlight,hidden,float(noise),walls);}
    Vector2 get_position()const{return vec(ai.position);}
    String get_state()const{const char* names[]={"PATROL","INVESTIGATE","CHASE","RETURN"};return names[ai.state];}
};
/// Exposes the exact GLM matrices used by our native transformation pipeline.
class TLSDebugStats : public RefCounted {
    GDCLASS(TLSDebugStats,RefCounted)
protected:
    static void _bind_methods(){ClassDB::bind_method(D_METHOD("matrices","player","angle","camera","zoom","size"),&TLSDebugStats::matrices);}
public:
    Dictionary matrices(Vector2 player,double angle,Vector2 camera,double zoom,Vector2 size)const{
        tls::Camera2D c;c.center=vec(camera);c.zoom=float(zoom);Dictionary d;
        auto put=[&](const char* key,const glm::mat4& m){PackedFloat32Array a;for(int r=0;r<4;++r)for(int col=0;col<4;++col)a.push_back(m[col][r]);d[key]=a;};
        put("model",tls::Camera2D::model(vec(player),{32,32}));put("flashlight_model",tls::Camera2D::model(vec(player),{32,32},float(angle)));put("view",c.view());put("projection",c.projection(float(size.x),float(size.y)));return d;
    }
};
/// Render-thread GL integration plus explicit CPU visibility texture fallback.
class TLSLightingRenderer : public Node {
    GDCLASS(TLSLightingRenderer,Node)
    struct Frame {tls::Camera2D camera;std::vector<tls::Light> lights;std::vector<glm::vec4> ui;float time=0,threat=0;int width=1280,height=720;};
    std::mutex mutex;Frame frame;std::vector<tls::Segment> walls;
    std::unique_ptr<tls::Renderer> renderer;
    std::atomic<bool> active{false},raw{true},failed{false};std::atomic<int> frames{0},draws{0},last_error{0};
    Ref<ImageTexture> mask;String error,capture_path;
protected:
    static void _bind_methods(){
        ClassDB::bind_method(D_METHOD("set_walls","edges"),&TLSLightingRenderer::set_walls);
        ClassDB::bind_method(D_METHOD("set_frame","player","angle","camera","zoom","flashlight","threat","lamps","time","size"),&TLSLightingRenderer::set_frame);
        ClassDB::bind_method(D_METHOD("set_active","enabled"),&TLSLightingRenderer::set_active);
        ClassDB::bind_method(D_METHOD("set_raw","enabled"),&TLSLightingRenderer::set_raw);
        ClassDB::bind_method(D_METHOD("get_stats"),&TLSLightingRenderer::get_stats);
        ClassDB::bind_method(D_METHOD("get_mask"),&TLSLightingRenderer::get_mask);
        ClassDB::bind_method(D_METHOD("request_capture","path"),&TLSLightingRenderer::request_capture);
        ClassDB::bind_method(D_METHOD("set_ui_regions","rectangles"),&TLSLightingRenderer::set_ui_regions);
        ClassDB::bind_method(D_METHOD("_post_draw"),&TLSLightingRenderer::post_draw);
        ClassDB::bind_method(D_METHOD("_render_pass"),&TLSLightingRenderer::render_pass);
        ClassDB::bind_method(D_METHOD("_cleanup"),&TLSLightingRenderer::cleanup);
    }
public:
    void _ready() override {RenderingServer::get_singleton()->connect("frame_post_draw",Callable(this,"_post_draw"));}
    void _exit_tree() override {
        active=false;auto server=RenderingServer::get_singleton();server->disconnect("frame_post_draw",Callable(this,"_post_draw"));server->call_on_render_thread(Callable(this,"_cleanup"));server->force_sync();
    }
    void set_walls(PackedVector2Array data){std::lock_guard<std::mutex> lock(mutex);walls=segments(data);}
    void set_active(bool enabled){active=enabled;}
    void set_raw(bool enabled){raw=enabled;}
    void request_capture(String path){std::lock_guard<std::mutex> lock(mutex);capture_path=path;}
    void set_ui_regions(PackedFloat32Array rectangles){std::lock_guard<std::mutex> lock(mutex);frame.ui.clear();for(int64_t i=0;i+3<rectangles.size()&&frame.ui.size()<5;i+=4)frame.ui.emplace_back(rectangles[i],rectangles[i+1],rectangles[i+2],rectangles[i+3]);}
    void set_frame(Vector2 player,double angle,Vector2 camera,double zoom,bool flashlight,double threat,PackedVector2Array lamps,double time,Vector2 size){
        std::lock_guard<std::mutex> lock(mutex);frame.camera.center=vec(camera);frame.camera.zoom=float(zoom);frame.time=float(time);frame.threat=float(threat);frame.width=int(size.x);frame.height=int(size.y);frame.lights.clear();
        // Keep a small personal radius so a switched-off torch remains navigable.
        frame.lights.push_back({vec(player),92,0,-1,.65f,{.57f,.69f,.77f}});
        if(flashlight) frame.lights.push_back({vec(player),440,float(angle),.76f,2.4f-float(threat)*.45f,{.84f,.93f,1.f}});
        const float blackout=std::fmod(float(time),31.f)>29.8f?.08f:1.f;
        for(auto p:lamps) if(glm::length(vec(p)-frame.camera.center)<900) frame.lights.push_back({vec(p),245,0,-1,.95f*blackout,{.63f,.77f,.69f}});
    }
    void post_draw(){if(active && raw && !failed) RenderingServer::get_singleton()->call_on_render_thread(Callable(this,"_render_pass"));}
    void render_pass(){
        if(!active || !raw || failed) return;
        try{
            if(!renderer){std::string message;if(!tls::initialize_gl(message)){error=message.c_str();failed=true;return;}}
            tls::GLStateGuard guard;
            std::lock_guard<std::mutex> lock(mutex);
            GLint framebuffer,viewport[4];glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING,&framebuffer);glGetIntegerv(GL_VIEWPORT,viewport);
            if(viewport[2]<=0||viewport[3]<=0)return;
            if(!renderer) renderer=std::make_unique<tls::Renderer>();
            renderer->resize(viewport[2],viewport[3]);renderer->capture(framebuffer,viewport[0],viewport[1]);
            // Camera coordinates are scaled to the actual drawable size (HiDPI/resizing).
            auto camera=frame.camera;camera.zoom*=float(viewport[2])/float(std::max(1,frame.width));
            renderer->composite(framebuffer,camera,walls,frame.lights,frame.time,frame.threat,viewport[0],viewport[1],frame.ui);
            draws=renderer->draw_calls;++frames;last_error=glGetError();
            if(!capture_path.is_empty()){
                PackedByteArray pixels;pixels.resize(viewport[2]*viewport[3]*4);glPixelStorei(GL_PACK_ALIGNMENT,1);
                glReadPixels(viewport[0],viewport[1],viewport[2],viewport[3],GL_RGBA,GL_UNSIGNED_BYTE,pixels.ptrw());
                Ref<Image> image=Image::create_from_data(viewport[2],viewport[3],false,Image::FORMAT_RGBA8,pixels);image->flip_y();image->save_png(capture_path);capture_path="";
            }
        }catch(const std::exception& e){error=e.what();failed=true;}
    }
    void cleanup(){renderer.reset();}
    Dictionary get_stats()const{Dictionary d;const bool has_failed=failed.load();d["path"]=raw&&!has_failed?"RAW OPENGL":"NATIVE VISIBILITY TEXTURE";d["frames"]=frames.load();d["draw_calls"]=draws.load();d["gl_error"]=last_error.load();d["failed"]=has_failed;d["error"]=has_failed?error:String();return d;}
    Ref<ImageTexture> get_mask(){
        std::lock_guard<std::mutex> lock(mutex);const int w=256,h=144;PackedByteArray bytes;bytes.resize(w*h*4);auto dst=bytes.ptrw();
        const glm::vec2 size(float(frame.width),float(frame.height));
        for(int y=0;y<h;++y)for(int x=0;x<w;++x){
            auto p=frame.camera.center+(glm::vec2((x+.5f)/w,(y+.5f)/h)-.5f)*size/frame.camera.zoom;glm::vec3 rgb(.17f,.20f,.24f);
            for(const auto& l:frame.lights){auto delta=p-l.position;float len=glm::length(delta);if(len>=l.radius)continue;float beam=1;
                if(l.cone>=0){float dot=glm::dot(delta/std::max(.001f,len),glm::vec2(std::cos(l.angle),std::sin(l.angle)));float t=std::clamp((dot-(l.cone-.16f))/.26f,0.f,1.f);beam=t*t*(3-2*t);}
                if(beam<.001f || !tls::Visibility::line_of_sight(l.position,p,walls))continue;
                float flicker=.96f+.04f*std::sin(frame.time*31+std::sin(frame.time*7));rgb+=l.color*std::pow(1-len/l.radius,1.3f)*beam*l.strength*flicker;
            }
            int i=(y*w+x)*4;for(int c=0;c<3;++c)dst[i+c]=(uint8_t)(std::clamp(rgb[c],0.f,1.f)*255);dst[i+3]=255;
        }
        Ref<Image> image=Image::create_from_data(w,h,false,Image::FORMAT_RGBA8,bytes);
        if(mask.is_null())mask=ImageTexture::create_from_image(image);else mask->update(image);return mask;
    }
};
void initialize_tls(ModuleInitializationLevel level){if(level!=MODULE_INITIALIZATION_LEVEL_SCENE)return;ClassDB::register_class<TLSVisibility>();ClassDB::register_class<TLSShadeAI>();ClassDB::register_class<TLSDebugStats>();ClassDB::register_class<TLSLightingRenderer>();}
void uninitialize_tls(ModuleInitializationLevel){}
extern "C" GDExtensionBool GDE_EXPORT tls_library_init(GDExtensionInterfaceGetProcAddress get_proc,GDExtensionClassLibraryPtr library,GDExtensionInitialization* initialization){
    GDExtensionBinding::InitObject init(get_proc,library,initialization);init.register_initializer(initialize_tls);init.register_terminator(uninitialize_tls);init.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);return init.init();
}
