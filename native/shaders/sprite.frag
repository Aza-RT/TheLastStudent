#version 330 core
in vec2 uv; in vec4 tint; out vec4 frag;
uniform sampler2D atlas;
void main(){ frag=texture(atlas,uv)*tint; }
