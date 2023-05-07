#version 450 core

layout (location=0) in vec3 a_pos;
layout (location=1) in vec4 a_color;

layout (location=0) uniform mat4 proj;
layout (location=1) uniform mat4 view;

out gl_PerVertex { vec4 gl_Position; };
out vec4 v_color;

void main() {
  gl_Position = proj * view * vec4(a_pos, 1);
  v_color = a_color;
}
