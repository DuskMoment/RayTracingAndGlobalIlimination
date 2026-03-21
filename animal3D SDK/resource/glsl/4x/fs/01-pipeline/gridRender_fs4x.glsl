#version 450

in vec4 vTexcoord_atlas;

uniform vec4 uColor;

//uniform float density[];

layout (location = 0) out vec4 rtFragColor;


void main()
{
	// DUMMY OUTPUT: all fragments are OPAQUE YELLOW
//	rtFragColor = vec4(1.0, 1.0, 0.0, 1.0);

	rtFragColor = vec4(1.0);
}
