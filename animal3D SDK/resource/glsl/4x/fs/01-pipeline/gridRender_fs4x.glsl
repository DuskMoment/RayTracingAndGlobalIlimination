#version 450

in vec4 vTexcoord_atlas;

uniform vec4 uColor;

//uniform float density[];

layout (location = 0) out vec4 rtFragColor;

int N = 3;

uniform density {
	float d[5 * 5];
};

int fragCoordToGridIndex(int i, int j)
{
	return i + (j * (N+2));
}

void main()
{
	// DUMMY OUTPUT: all fragments are OPAQUE YELLOW
//	rtFragColor = vec4(1.0, 1.0, 0.0, 1.0);

	//float col = d[fragCoordToGridIndex(int(gl_FragCoord.x * N), int(gl_FragCoord.y * N))];
	float col = d[fragCoordToGridIndex(int(0), int(0))];

	rtFragColor = vec4(col, col, col, 1.0);
}
