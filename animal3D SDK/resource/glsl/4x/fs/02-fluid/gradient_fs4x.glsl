#version 450

in vec4 vTexcoord_atlas;

const float screenRecip = 0.001253133;

uniform sampler2D uImage00; //prev velocity(xyz) & density(a)
uniform sampler2D uImage01; //divergence(x) & pressure(y)

layout (location = 0) out vec4 current;

void main()
{
	float u = texture(uImage00, vTexcoord_atlas.xy).x;
	float v = texture(uImage00, vTexcoord_atlas.xy).y;

	u -= 0.5 * (texture(uImage01, vec2(vTexcoord_atlas.x + screenRecip, vTexcoord_atlas.y)).y - 
               texture(uImage01, vec2(vTexcoord_atlas.x - screenRecip, vTexcoord_atlas.y)).y) * screenRecip;

	v -= 0.5 * (texture(uImage01, vec2(vTexcoord_atlas.x, vTexcoord_atlas.y  + screenRecip)).y - 
               texture(uImage01, vec2(vTexcoord_atlas.x, vTexcoord_atlas.y  - screenRecip)).y) * screenRecip;

	current.rg = vec2(u, v);
}
