#version 450 core

in vec4 v_color;

layout (location=0) out vec4 o_color;

void main() {
  o_color = v_color;
}
