#version 330 core
in vec2 world; out vec4 frag;
uniform vec2 origin; uniform float radius,angle,cone,strength,time;
uniform vec3 light_color;
// A stable hash interpolated across time, avoiding random-per-frame harsh flicker.
float hash(float n){ return fract(sin(n*127.1)*43758.5453); }
void main(){
 vec2 delta=world-origin; float distance_to_light=length(delta);
 // Squared radial attenuation and a soft angular penumbra.
 float falloff=pow(max(0.0,1.0-distance_to_light/radius),1.3);
 float direction=dot(normalize(delta+vec2(.00001)),vec2(cos(angle),sin(angle)));
 float beam=cone<0.0?1.0:smoothstep(cone-.16,cone+.10,direction);
 float t=time*11.0; float flicker=mix(hash(floor(t)),hash(floor(t)+1.0),smoothstep(0.,1.,fract(t)));
 frag=vec4(light_color*falloff*beam*strength*(.92+.08*flicker),1.0);
}
