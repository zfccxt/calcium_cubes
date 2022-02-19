#version 450

layout (location = 0) out vec4 o_colour;

layout (location = 0) in vec2 v_tex;
layout (location = 1) flat in float v_norm;

layout (binding = 1) uniform sampler2D u_texture;

void main() {
  o_colour = texture(u_texture, v_tex);
  if (o_colour.a < 0.5) {
    discard;
  }
  o_colour.rgb *= v_norm;
}
