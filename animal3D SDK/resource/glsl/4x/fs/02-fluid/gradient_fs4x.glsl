#version 450

in vec4 vTexcoord_atlas;

const float screenRecip = 0.001253133;

uniform sampler2D uImage00; //prev velocity(xyz) & density(a)
uniform sampler2D uImage01; //divergence(x) & pressure(y)

layout (location = 0) out vec4 current;

void main()
{
	float u = texture(uImage00, vTexcoord_atlas.xy).x * 2.0 - 1.0;
	float v = texture(uImage00, vTexcoord_atlas.xy).y * 2.0 - 1.0;

	vec4 LU = texture(uImage01, vec2(vTexcoord_atlas.x - screenRecip, vTexcoord_atlas.y)) * 2.0 - 1.0;
	vec4 RU = texture(uImage01, vec2(vTexcoord_atlas.x + screenRecip, vTexcoord_atlas.y)) * 2.0 - 1.0;
	u -= 0.5 * (RU.x - LU.x) * screenRecip;

	vec4 TV = texture(uImage01, vec2(vTexcoord_atlas.x, vTexcoord_atlas.y  + screenRecip)) * 2.0 - 1.0;
	vec4 BV = texture(uImage01, vec2(vTexcoord_atlas.x, vTexcoord_atlas.y  - screenRecip)) * 2.0 - 1.0;
	v -= 0.5 * (TV.y - BV.y) * screenRecip;

	current = vec4(u * 0.5 + 0.5, v * 0.5 + 0.5, 0.0, 1.0);
}
