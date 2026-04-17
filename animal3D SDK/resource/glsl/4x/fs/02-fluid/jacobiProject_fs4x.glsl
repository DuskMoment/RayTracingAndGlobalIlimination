#version 450

in vec4 vTexcoord_atlas;

const float screenRecip = 0.00125;

uniform sampler2D uImage00; //divergence and pressure

layout (location = 0) out vec4 current;

void main()
{
  float p = texture(uImage00, vTexcoord_atlas.xy).x +
            texture(uImage00, vec2(vTexcoord_atlas.x - screenRecip, vTexcoord_atlas.y)).y +
            texture(uImage00, vec2(vTexcoord_atlas.x + screenRecip, vTexcoord_atlas.y)).y +
            texture(uImage00, vec2(vTexcoord_atlas.x, vTexcoord_atlas.y - screenRecip)).y +
            texture(uImage00, vec2(vTexcoord_atlas.x, vTexcoord_atlas.y + screenRecip)).y;

  p *= 0.25;

  current.y = p;
}
