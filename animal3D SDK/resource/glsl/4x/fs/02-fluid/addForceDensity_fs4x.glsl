#version 450

in vec4 vTexcoord_atlas;

const float screenRecip = 0.00125;
uniform float dt; 

uniform sampler2D uImage00; //prev density
uniform sampler2D uImage01; //add density

layout (location = 0) out vec4 current;

void main()
{
	vec4 col = texture(uImage00, vTexcoord_atlas.xy);
	col += texture(uImage01, vTexcoord_atlas.xy);
	col = max(vec4(0.0), min(vec4(1.0), col));
	current =  col;
}
	
