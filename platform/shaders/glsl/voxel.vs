#version 450 core

layout (location=0) in vec3 a_pos;
layout (location=1) in vec3 a_nor;
layout (location=2) in vec2 a_uv;

layout (location=0) uniform mat4 proj;
layout (location=1) uniform mat4 view;

out gl_PerVertex { vec4 gl_Position; };
out vec2 uv;
out vec3 normal;

void main() {
  gl_Position = proj * view * vec4(a_pos, 1);
  uv = a_uv;
  normal = a_nor;
}