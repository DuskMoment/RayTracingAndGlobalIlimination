#version 450

in vec4 vTexcoord_atlas;

uniform float uScreenRecip = 0.00125;

uniform float a; //dt * diff * N * N
uniform float diffuseDenom; // = 1.0 / (a * 4 + 1)

uniform float uToRangeM;
uniform float uToRangeS;
uniform float uToColorM; 
uniform float uToColorA; 

uniform sampler2D uImage00; //prev 
uniform sampler2D uImage01; //current 

layout (location = 0) out vec4 current;

void main()
{
 vec4 adjDiff = (texture(uImage01, vec2(vTexcoord_atlas.x - uScreenRecip, vTexcoord_atlas.y)) * uToRangeM - uToRangeS) +
                (texture(uImage01, vec2(vTexcoord_atlas.x + uScreenRecip, vTexcoord_atlas.y)) * uToRangeM - uToRangeS) +
                (texture(uImage01, vec2(vTexcoord_atlas.x, vTexcoord_atlas.y - uScreenRecip)) * uToRangeM - uToRangeS) +
                (texture(uImage01, vec2(vTexcoord_atlas.x, vTexcoord_atlas.y + uScreenRecip)) * uToRangeM - uToRangeS);

  vec4 numerator = (texture(uImage00, vTexcoord_atlas.xy) * uToRangeM - uToRangeS) + a * adjDiff;
  vec4 col = numerator * diffuseDenom;
  current = vec4(col.xyz * uToColorM + uToColorA, 1.0);
}
