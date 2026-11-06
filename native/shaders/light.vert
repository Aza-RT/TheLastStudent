#version 330 core
layout(location=0) in vec2 position;
uniform mat4 view_projection;
out vec2 world;
void main(){ world=position; gl_Position=view_projection*vec4(position,0,1); }
