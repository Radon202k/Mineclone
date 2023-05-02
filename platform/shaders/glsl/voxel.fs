#version 450 core

in vec2 uv;

layout (binding=0) uniform sampler2D sampler;

layout (location=0) out vec4 o_color;

void main() {
  o_color = texture(sampler, uv);
}