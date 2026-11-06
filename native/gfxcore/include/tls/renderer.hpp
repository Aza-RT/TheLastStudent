#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <tls/visibility.hpp>
#include <string>
#include <unordered_map>
#include <vector>
namespace tls {
/// Compiled GLSL program with cached uniform locations and RAII ownership.
class Shader {
    GLuint id_=0; std::unordered_map<std::string,GLint> uniforms_;
public:
    /// Construct empty; compile only after a current GL context exists.
    Shader()=default; ~Shader(); Shader(const Shader&)=delete; Shader& operator=(const Shader&)=delete;
    /// Atomically replace a linked program, throwing its complete compile/link error.
    void compile(const char* vertex,const char* fragment);
    /// Bind this program for subsequent draw calls.
    void use() const { glUseProgram(id_); }
    /// Cache and return a named uniform location (including inactive -1 results).
    GLint uniform(const char* name);
};
/// Nearest-filtered RGBA texture. Images are decoded by stb_image.
class Texture2D {
public:
    GLuint id=0; int width=0,height=0;
    /// Create/delete one GL texture; the caller owns the current context lifetime.
    Texture2D(); ~Texture2D(); Texture2D(const Texture2D&)=delete; Texture2D& operator=(const Texture2D&)=delete;
    /// Allocate RGBA8 storage and nearest/clamped texture sampling.
    void allocate(int w,int h,const void* pixels=nullptr);
    /// Decode an RGBA image with stb_image and upload it, or throw on failure.
    void load(const std::string& path);
};
/// Color FBO, resized only when the viewport changes.
class Framebuffer {
public:
    GLuint id=0; Texture2D color;
    /// Create/delete the FBO, with its color texture owned as a member.
    Framebuffer(); ~Framebuffer(); Framebuffer(const Framebuffer&)=delete;
    /// Resize its attachment only when needed and verify framebuffer completeness.
    void resize(int w,int h);
};
/// Streaming indexed mesh. Capacities grow only when necessary.
class Mesh {
public:
    GLuint vao=0,vbo=0,ebo=0;
    /// Allocate/release a VAO and vertex/index buffers in the caller's context.
    Mesh(); ~Mesh(); Mesh(const Mesh&)=delete;
};
/// Explicit GLM model -> view -> orthographic projection camera.
struct Camera2D {
    glm::vec2 center{0}; float zoom=1;
    /// Convert world positions into a zoomed view centered on center.
    glm::mat4 view() const { return glm::scale(glm::mat4(1),glm::vec3(zoom,zoom,1))*glm::translate(glm::mat4(1),glm::vec3(-center,0)); }
    /// Orthographic projection with positive world Y pointing down.
    glm::mat4 projection(float w,float h) const { return glm::ortho(-w/2,w/2,h/2,-h/2,-1.f,1.f); }
    /// Compose translation, aim rotation and quad size in column-vector order.
    static glm::mat4 model(glm::vec2 p,glm::vec2 size,float angle=0) {
        return glm::translate(glm::mat4(1),glm::vec3(p,0))*glm::rotate(glm::mat4(1),angle,glm::vec3(0,0,1))*glm::scale(glm::mat4(1),glm::vec3(size,1));
    }
};
struct Vertex { glm::vec2 position,uv; glm::vec4 color; };
/// One draw call for an atlas of transformed textured sprites.
class SpriteBatch {
    Mesh mesh_; Shader shader_; std::vector<Vertex> vertices_; std::vector<unsigned> indices_;
public:
    /// Compile the atlas shader and reserve a reusable streaming batch.
    SpriteBatch();
    /// Clear retained CPU buffers without shrinking capacity.
    void begin();
    /// Append a transformed unit quad with normalized atlas UV bounds.
    void quad(const glm::mat4& model,glm::vec4 uv,glm::vec4 color=glm::vec4(1));
    /// Upload and draw the entire indexed batch with one atlas binding.
    void flush(const Texture2D& atlas,const glm::mat4& vp);
};
/// Every state modified by our pass is captured and restored, including separate read/draw FBOs.
class GLStateGuard {
    GLint program_,vao_,array_,element_,active_,texture_,texture0_,draw_,read_,viewport_[4],scissor_[4];
    GLint src_rgb_,dst_rgb_,src_alpha_,dst_alpha_,eq_rgb_,eq_alpha_,pack_,unpack_;
    GLfloat clear_color_[4];
    GLboolean blend_,depth_,scissor_on_,cull_,srgb_,depth_mask_,color_mask_[4];
public:
    /// Snapshot/restore exactly the state touched by our native renderer.
    GLStateGuard(); ~GLStateGuard();
};
/// A native world-space light; negative cone means omnidirectional.
struct Light { glm::vec2 position; float radius=300, angle=0, cone=-1, strength=1; glm::vec3 color{1}; };
/// Visibility polygons -> additive light FBO -> textured scene + postprocess composite.
class Renderer {
    Shader light_,post_; Mesh polygon_,fullscreen_;
    std::vector<glm::vec2> boundary_,fan_;
public:
    Framebuffer scene,illumination;
    int width=0,height=0,draw_calls=0;
    /// Compile embedded GLSL and create reusable FBOs/meshes in the current context.
    Renderer();
    /// Resize color/light targets when drawable size changes.
    void resize(int w,int h);
    /// Bind and clear the standalone scene target for sprite rendering.
    void begin_scene(glm::vec3 clear);
    /// Copy the engine's already-drawn color into the owned scene texture.
    void capture(GLuint source,int x,int y);
    /// Build wall-clipped light fans, draw their FBO, then shade/postprocess the scene.
    void composite(GLuint destination,const Camera2D& camera,const std::vector<Segment>& walls,const std::vector<Light>& lights,float time,float threat,int x=0,int y=0,const std::vector<glm::vec4>& ui_regions={});
};
/// Initialize GLEW against the already-current OpenGL context, never creating a second context.
bool initialize_gl(std::string& error);
}
