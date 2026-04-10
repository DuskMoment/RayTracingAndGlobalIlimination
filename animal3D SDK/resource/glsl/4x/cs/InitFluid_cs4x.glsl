#version 450

//CHANGE THE GRIDSIZE HERE 
const int GRID_LENGHT = 798;
const int GRID_SIZE = (GRID_LENGHT + 2) * (GRID_LENGHT + 2);


//THIS SHOULD ONLY HAVE ONE WORK GROUP
layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

layout(std430, binding = 1) buffer inputBuffer {

	float inputGrid[GRID_SIZE];
};

uniform float uDeltaTime;
uniform float uPixelRadius;
uniform vec2 uPoint;

void main()
{
	vec3 clusterSize = gl_NumWorkGroups * gl_WorkGroupSize;

	uint index = uint(gl_GlobalInvocationID.x + gl_GlobalInvocationID.y * clusterSize.x);

	inputGrid[index] = 0;
}


