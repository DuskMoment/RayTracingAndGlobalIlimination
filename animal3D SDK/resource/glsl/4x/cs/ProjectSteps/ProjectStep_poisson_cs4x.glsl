#version 450
#define IX2(i,j) (i + ((gl_NumWorkGroups * gl_WorkGroupSize).x * j)) 
/*
THIS RUNS PER PIXEL --> run this 20 times the gpu
and then also in between each loop use SETBND
*/


layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;
//CHANGE THE GRIDSIZE HERE 
const int GRID_LENGHT = 798;
const int GRID_SIZE = (GRID_LENGHT + 2) * (GRID_LENGHT + 2);

//presure
layout(std430, binding = 1) buffer bufferInOutX {

	float x[];
};

//divergance
layout(std430, binding = 2) buffer bufferInOutX0 {

	float x0[];
};


void main()
{
	int j = int(gl_GlobalInvocationID.y), 
	i  = int(gl_GlobalInvocationID.x),
	N = GRID_LENGHT;

	x[IX2(i, j)] = (x0[IX2(i, j)] + x[IX2(i - 1, j)] + x[IX2(i + 1, j)] + x[IX2(i, j - 1)] + x[IX2(i, j + 1)]) / 4.0;
	

	//CALL BND ON CPU
	//ASLO RELAX

}