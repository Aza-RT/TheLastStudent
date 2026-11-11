#version 330 core
in vec2 uv;
out vec4 frag;

const float LINE_PERIOD = 3.0;   
const float LINE_ALPHA  = 0.20;  
void main() {
    float row = mod(floor(gl_FragCoord.y), LINE_PERIOD);
    float alpha = (row < 1.0) ? LINE_ALPHA : 0.0;
    frag = vec4(0.0, 0.0, 0.0, alpha);
}