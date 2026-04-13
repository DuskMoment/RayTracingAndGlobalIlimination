#version 450

layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

layout(std430, binding = 1) buffer buf_in {

	float data_in[];
};

uniform float uDt;

void main()
{
	vec3 clusterSize = gl_NumWorkGroups * gl_WorkGroupSize;

	uint index = uint(gl_GlobalInvocationID.x + gl_GlobalInvocationID.y * clusterSize.x);
	

	float test = data_in[index];
	test = test - 0.1 * uDt;


	test = clamp(test, 0.0, 11.0);

	data_in[index] = test;

}