#version 450
#define IX2(i,j) ((i)+(N+2)*(j)) 
/*
THIS RUNS PER PIXEL --> run this 20 times the gpu
and then also in between each loop use SETBND
*/


layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;
//CHANGE THE GRIDSIZE HERE 
const int GRID_LENGHT = 798;
const int GRID_SIZE = (GRID_LENGHT + 2) * (GRID_LENGHT + 2);

layout(std430, binding = 1) buffer bufferInOutU {

	float u[];
};

layout(std430, binding = 2) buffer bufferInOutV {

	float v[];
};


//presure
layout(std430, binding = 3) buffer bufferInOutU0 {

	float p[];
};

//divergance
layout(std430, binding = 4) buffer bufferInOutV0 {

	float div[];
};


void main()
{

	
	int j = int(gl_GlobalInvocationID.y) , 
	i  = int(gl_GlobalInvocationID.x),
	N = GRID_LENGHT;


	float h;
    h = float(1.0) / float(N);


	div[IX2(i, j)] =  float(-0.5) * h * (u[IX2(i + 1, j)] - u[IX2(i - 1, j)] + v[IX2(i, j + 1)] - v[IX2(i, j - 1)]);
    p[IX2(i, j)] = 0;

	
	
	//CALL BND ON CPU

}