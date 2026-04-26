#version 450

in vec4 vTexcoord_atlas;

uniform float uScreenRecip;

uniform sampler2D uImage00; //current velocity
uniform float uToRangeM;
uniform float uToRangeS;
uniform float uToColorM; 
uniform float uToColorA; 

layout (location = 0) out vec4 current;

void main()
{
    float div = float(-0.5) * uScreenRecip * 
                    ((texture(uImage00, vec2(vTexcoord_atlas.x + uScreenRecip, vTexcoord_atlas.y)).x * uToRangeM - uToRangeS) - 
                     (texture(uImage00, vec2(vTexcoord_atlas.x - uScreenRecip, vTexcoord_atlas.y)).x * uToRangeM - uToRangeS) +
                     (texture(uImage00, vec2(vTexcoord_atlas.x, vTexcoord_atlas.y + uScreenRecip)).y * uToRangeM - uToRangeS) - 
                     (texture(uImage00, vec2(vTexcoord_atlas.x, vTexcoord_atlas.y - uScreenRecip)).y * uToRangeM - uToRangeS));

    float p = 0.0;

    current = vec4(div * uToColorM + uToColorA, p, 0.0, 1.0);
}
