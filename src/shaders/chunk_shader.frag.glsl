#version 450

layout (location = 0) out vec4 o_colour;

layout (location = 0) in vec3 v_tex;
layout (location = 1) flat in float v_norm;

layout (binding = 1) uniform sampler2DArray u_block_texture_array;

void main() {
  o_colour = texture(u_block_texture_array, v_tex);
  if (o_colour.a < 0.5) {
    discard;
  }
  o_colour.rgb *= v_norm;
}
