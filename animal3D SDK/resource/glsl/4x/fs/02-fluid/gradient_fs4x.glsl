#version 450

in vec4 vTexcoord_atlas;

const float screenRecip = 0.00125;

uniform sampler2D uImage00; //prev velocity(xyz) & density(a)
uniform sampler2D uImage01; //divergence(x) & pressure(y)

uniform float uToRangeM;
uniform float uToRangeS;
uniform float uToColorM; 
uniform float uToColorA; 

layout (location = 0) out vec4 current;

void main()
{
	float u = texture(uImage00, vTexcoord_atlas.xy).x * uToRangeM - uToRangeS;
	float v = texture(uImage00, vTexcoord_atlas.xy).y * uToRangeM - uToRangeS;

	vec4 LU = texture(uImage01, vec2(vTexcoord_atlas.x - screenRecip, vTexcoord_atlas.y)) * uToRangeM - uToRangeS;
	vec4 RU = texture(uImage01, vec2(vTexcoord_atlas.x + screenRecip, vTexcoord_atlas.y)) * uToRangeM - uToRangeS;
	u -= 0.5 * (RU.x - LU.x) * screenRecip;

	vec4 TV = texture(uImage01, vec2(vTexcoord_atlas.x, vTexcoord_atlas.y  + screenRecip)) * uToRangeM - uToRangeS;
	vec4 BV = texture(uImage01, vec2(vTexcoord_atlas.x, vTexcoord_atlas.y  - screenRecip)) * uToRangeM - uToRangeS;
	v -= 0.5 * (TV.y - BV.y) * screenRecip;

	current = vec4(u * uToColorM + uToColorA, v * uToColorM + uToColorA, 0.0, 1.0);
}
