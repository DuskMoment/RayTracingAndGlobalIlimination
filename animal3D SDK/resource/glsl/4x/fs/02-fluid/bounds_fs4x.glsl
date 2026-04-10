#version 450

in vec4 vTexcoord_atlas;

uniform sampler2D uImage00; //current velocity
uniform sampler2D uImage01; //bounds negate texture

layout (location = 0) out vec4 current;

void main()
{
	current = texture(uImage01, vTexcoord_atlas.xy) * (texture(uImage01, vTexcoord_atlas.xy) * 2.0 - 1.0);
}
	
