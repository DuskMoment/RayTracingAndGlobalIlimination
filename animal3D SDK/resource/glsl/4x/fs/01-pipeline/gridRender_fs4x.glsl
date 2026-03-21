#version 450

in vec4 vTexcoord_atlas;

uniform vec4 uColor;

//uniform float density[];

layout (location = 0) out vec4 rtFragColor;

int N = 100;
uniform float density[102 * 102];

int fragCoordToGridIndex(int i, int j)
{
	return i + (j * (N+2));
}

void main()
{
	// DUMMY OUTPUT: all fragments are OPAQUE YELLOW
//	rtFragColor = vec4(1.0, 1.0, 0.0, 1.0);

	float density = density[fragCoordToGridIndex(int(gl_FragCoord.x * N), int(gl_FragCoord.y * N))];

	rtFragColor = vec4(density, density, density, 1.0);
}
