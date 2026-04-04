#version 450

layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

layout(std430, binding = 1) buffer densityBuf_out {

	float density_out[];
};

void main()
{
	uint index = gl_GlobalInvocationID.x;

	density_out[index] = 1;
}