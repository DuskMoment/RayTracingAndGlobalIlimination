#version 450

in vec4 vTexcoord_atlas;

const float screenRecip = 0.001253133;
uniform float dt; 

uniform sampler2D uImage00; //prev velocity(xyz) & density(a)
uniform sampler2D uImage01; //current velocity(xyz) & density(a)
uniform sampler2D uImage02; //current divergence(x) & pressure(y)
uniform sampler2D uImage03; //prev divergence(x) & pressure(y)

layout (location = 0) out vec4 current;

void main()
{
	vec4 col = texture(uImage00, vTexcoord_atlas.xy);
	col += texture(uImage01, vTexcoord_atlas.xy) * dt;
	col = max(vec4(0.0), min(vec4(1.0), col));
	current =  col;
}
	
