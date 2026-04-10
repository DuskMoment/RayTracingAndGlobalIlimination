#version 450

in vec4 vTexcoord_atlas;

uniform vec4 uColor;
uniform sampler2D uImage00;

//uniform float density[];

layout (location = 0) out vec4 rtFragColor;

layout(std430, binding = 1) buffer densityBuf_out {

	float density_out[];
};

int N = 798;

uniform density {
	float d[40 * 40];
};

int fragCoordToGridIndex(int i, int j)
{
	return i + (j * (N+2));
}

void main()
{
	// DUMMY OUTPUT: all fragments are OPAQUE YELLOW
//	rtFragColor = vec4(1.0, 1.0, 0.0, 1.0);

	float col = density_out[fragCoordToGridIndex(int(vTexcoord_atlas.x * N), int(vTexcoord_atlas.y * N))];
	//float col = d[fragCoordToGridIndex(int(0), int(0))];

	//col = min(col, 1);
	rtFragColor = vec4(texture(uImage00, vTexcoord_atlas.xy).r);
}
