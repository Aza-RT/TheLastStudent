shader_type canvas_item;
render_mode unshaded;
uniform sampler2D screen_texture : hint_screen_texture, filter_nearest;
uniform sampler2D native_mask : filter_linear;
uniform float threat = 0.0;
uniform float clock = 0.0;
float hash(vec2 p){return fract(sin(dot(p,vec2(12.9898,78.233)))*43758.5453);}
void fragment(){
    float tear=step(.965,hash(vec2(floor(SCREEN_UV.y*90.),floor(clock*12.))))*threat;
    vec2 p=clamp(SCREEN_UV+vec2(tear*.018*sin(clock*55.),0.),vec2(.001),vec2(.999));
    vec2 offset=vec2((.5+threat*3.)*SCREEN_PIXEL_SIZE.x,0);
    vec3 rgb=vec3(texture(screen_texture,p+offset).r,texture(screen_texture,p).g,texture(screen_texture,p-offset).b);
    float vignette=1.-.48*smoothstep(.20,.77,length(SCREEN_UV-.5));
    float grain=(hash(FRAGCOORD.xy+fract(clock)*173.)-.5)*.024;
    COLOR=vec4(max(vec3(0),rgb*texture(native_mask,SCREEN_UV).rgb*vignette+grain),1.);
}
