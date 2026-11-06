#version 330 core
layout(location=0) in vec2 position;
layout(location=1) in vec2 texcoord;
layout(location=2) in vec4 color;
uniform mat4 view_projection;
out vec2 uv; out vec4 tint;
void main(){ uv=texcoord; tint=color; gl_Position=view_projection*vec4(position,0,1); }
