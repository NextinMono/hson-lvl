#version 460 core
layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texcoord;

layout(location = 0) out vec2 vTexcoord;

layout(std140, binding = 0) uniform mainCbuffer {
    mat4 view;
    mat4 projection;
};

layout(push_constant) uniform modelParam {
    mat4 world;
};

void main() {
    vTexcoord = texcoord;
    gl_Position = projection * view * world * vec4(pos, 1.0);
}
