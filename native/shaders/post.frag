#version 330 core
in vec2 uv; out vec4 frag;
uniform sampler2D scene_tex,light_tex;
uniform float time,threat;
uniform vec2 resolution;
uniform int ui_rect_count;
uniform vec4 ui_rects[5]; // normalized top-left rectangles reserved for readable Godot HUD panels
float hash(vec2 p){return fract(sin(dot(p,vec2(12.9898,78.233)))*43758.5453);}
void main(){
 vec2 screen=vec2(uv.x,1.-uv.y);
 for(int i=0;i<ui_rect_count;++i){vec4 r=ui_rects[i];if(all(greaterThanEqual(screen,r.xy))&&all(lessThanEqual(screen,r.xy+r.zw))){frag=texture(scene_tex,uv);return;}}
 // Threat-driven scanline displacement and red/blue wavelength separation.
 float tear=step(.965,hash(vec2(floor(uv.y*90.),floor(time*12.))))*threat;
 vec2 p=clamp(uv+vec2(tear*.018*sin(time*55.),0),vec2(.001),vec2(.999));
 vec2 offset=vec2((.5+threat*3.)/resolution.x,0);
 vec3 rgb=vec3(texture(scene_tex,p+offset).r,texture(scene_tex,p).g,texture(scene_tex,p-offset).b);
 vec3 light=texture(light_tex,p).rgb;
 // Smooth radial vignette and zero-mean grain, evaluated in framebuffer pixels.
 float vignette=1.-.48*smoothstep(.20,.77,length(uv-.5));
 float grain=(hash(gl_FragCoord.xy+fract(time)*173.)-.5)*.024;
 frag=vec4(max(vec3(0),rgb*light*vignette+grain),1);
}
