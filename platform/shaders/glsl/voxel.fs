#version 450 core

in vec2 uv;
in vec3 normal;

layout (binding=0) uniform sampler2D sampler;

layout (location=0) out vec4 o_color;

uniform vec3 sunDirection = normalize(vec3(-1.0, 1.0, -1.0));
uniform vec3 sunColor = vec3(1.0, 1.0, 1.0);

void main() {
  vec3 N = normalize(normal);
  vec3 L = sunDirection;

  float diffuse = max(dot(N, L), 0.0);
  
  vec4 texel = texture(sampler, uv);

  vec3 ambient = 0.5 * texel.rgb;
  vec3 diffuseColor = diffuse * texel.rgb * sunColor;

  o_color = vec4(ambient + diffuseColor, texel.a);
}