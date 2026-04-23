#version 450

in vec4 vTexcoord_atlas;

const float screenRecip = 0.001253133;

uniform float dt;
uniform float fade;

uniform sampler2D uImage00; //prev velocity(xyz) & density(a)

layout (location = 0) out vec4 current;

void main()
{
	vec3 d;
	d = max(vec3(0.0), min(vec3(1.0), texture(uImage00, vTexcoord_atlas.xy).xyz));
    d -= dt * fade;

	current = vec4(d, 1.0);
}
