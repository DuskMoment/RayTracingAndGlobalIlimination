#version 450

in vec4 vTexcoord_atlas;

const float screenRecip = 0.00125;

uniform sampler2D uImage00; //divergence and pressure
uniform float uToRangeM;
uniform float uToRangeS;
uniform float uToColorM; 
uniform float uToColorA; 

layout (location = 0) out vec4 current;

void main()
{
  float p = (texture(uImage00, vTexcoord_atlas.xy).x * uToRangeM - uToRangeS) +
            (texture(uImage00, vec2(vTexcoord_atlas.x - screenRecip, vTexcoord_atlas.y)).x * uToRangeM - uToRangeS) +
            (texture(uImage00, vec2(vTexcoord_atlas.x + screenRecip, vTexcoord_atlas.y)).x * uToRangeM - uToRangeS) +
            (texture(uImage00, vec2(vTexcoord_atlas.x, vTexcoord_atlas.y - screenRecip)).y * uToRangeM - uToRangeS) +
            (texture(uImage00, vec2(vTexcoord_atlas.x, vTexcoord_atlas.y + screenRecip)).y * uToRangeM - uToRangeS) ;

  float div = texture(uImage00, vTexcoord_atlas.xy).x;
  p *= 0.25;

  current = vec4(div, p * uToColorM + uToColorA, 0.0, 1.0);
}
