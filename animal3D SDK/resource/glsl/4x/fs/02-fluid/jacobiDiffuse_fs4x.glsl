#version 450

in vec4 vTexcoord_atlas;

const float screenRecip = 0.00125;

uniform float a; //dt * diff * N * N
uniform float diffuseDenom; // = 1.0 / (a * 4 + 1)

uniform sampler2D uImage00; //prev 
uniform sampler2D uImage01; //current 

layout (location = 0) out vec4 current;

void main()
{
 vec4 adjDiff = (texture(uImage01, vec2(vTexcoord_atlas.x - screenRecip, vTexcoord_atlas.y)) * 2.0 - 1.0) +
                (texture(uImage01, vec2(vTexcoord_atlas.x + screenRecip, vTexcoord_atlas.y)) * 2.0 - 1.0) +
                (texture(uImage01, vec2(vTexcoord_atlas.x, vTexcoord_atlas.y - screenRecip)) * 2.0 - 1.0) +
                (texture(uImage01, vec2(vTexcoord_atlas.x, vTexcoord_atlas.y + screenRecip)) * 2.0 - 1.0);

  vec4 numerator = (texture(uImage00, vTexcoord_atlas.xy) * 2.0 - 1.0) + a * adjDiff;
  vec4 col = numerator * diffuseDenom;
  current = vec4(col.xyz * 0.5 + 0.5, 1.0);
}
