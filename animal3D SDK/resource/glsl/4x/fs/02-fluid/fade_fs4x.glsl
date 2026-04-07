#version 450

in vec4 vTexcoord_atlas;

const float screenRecip = 0.001253133;

uniform float dt;
uniform float fade;

uniform sampler2D uImage00; //prev velocity(xyz) & density(a)
uniform sampler2D uImage01; //current velocity(xyz) & density(a)
uniform sampler2D uImage02; //divergence(x) & pressure(y)
uniform sampler2D uImage03; //prev divergence(x) & pressure(y)

layout (location = 0) out vec4 current;

void main()
{
	float d;
	d = max(0.0, min(1.0, texture(uImage00, vTexcoord_atlas.xy).a));
    d -= dt * fade;

	current.a = d;
}
