#version 460 core
layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texcoord;

layout(location = 0) out vec2 vTexcoord;

void main() {
    vTexcoord = texcoord;
    gl_Position = vec4(pos, 1.0);
}
