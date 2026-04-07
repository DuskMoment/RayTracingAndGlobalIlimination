#version 450

in vec4 vTexcoord_atlas;

const float screenRecip = 0.001253133;
uniform bool velocity;
uniform bool div;
uniform float scale; //-1 for velocity, 1 for pressure
uniform vec2 offset;

uniform sampler2D uImage00; //prev velocity(xyz) & density(a)
uniform sampler2D uImage01; //current velocity(xyz) & density(a)
uniform sampler2D uImage02; //current divergence(x) & pressure(y)
uniform sampler2D uImage03; //prev divergence(x) & pressure(y)

layout (location = 0) out vec4 current;

void main()
{
	if (velocity)
	{
		vec2 b = -1.0 * texture(uImage01, vTexcoord_atlas.xy + offset).xy;
		current.xy = b;
	}
	else if (div)
	{
		vec2 b = texture(uImage02, vTexcoord_atlas.xy + offset).xy;
		current.xy = b;
	}
	else
	{
		float b = texture(uImage02, vTexcoord_atlas.xy + offset).y;
		current.y = b;
	}
}
	
