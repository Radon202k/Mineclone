#version 450 core

in vec2 uv;
in vec3 normal;

layout (binding=0) uniform sampler2D sampler;

layout (location=0) out vec4 o_color;

void main() {
  o_color = vec4(normal,1) * texture(sampler, uv);
}