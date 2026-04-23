#version 450

in vec4 vTexcoord_atlas;

uniform vec4 uColor;

uniform sampler2D uImage00;

layout (location = 0) out vec4 rtFragColor;

void main()
{
	float col = texture(uImage00, vTexcoord_atlas.xy).x;
	rtFragColor = vec4(col, col, col, 1.0);
}