#version 450

layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

layout(std430, binding = 1) buffer densityBuf_out {

	float density_out[];
};

void main()
{
	 vec3 clusterSize = gl_NumWorkGroups * gl_WorkGroupSize;

	uint index = uint(gl_GlobalInvocationID.x + gl_GlobalInvocationID.y * clusterSize.x);

	if (density_out[index] < 0)
	{
		density_out[index] = 1;
	}
	else {
		density_out[index] += 1;
	}

}