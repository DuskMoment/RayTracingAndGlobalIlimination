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

	float float_bufferInOutU[];
};

layout(std430, binding = 2) buffer bufferInOutV {

	float float_bufferInOutV[];
};


//presure
layout(std430, binding = 3) buffer bufferInOutU0 {

	float float_bufferInOutU0[];
};

//divergance
layout(std430, binding = 4) buffer bufferInOutV0 {

	float float_bufferInOutV0[];
};


void main()
{

	
	int j = int(gl_GlobalInvocationID.y) , 
	i  = int(gl_GlobalInvocationID.x),
	N = GRID_LENGHT;


	float h;
    h = 1.0 / float(N);
	
	float_bufferInOutV0[IX2(i, j)] = (-0.5) * h * 
	(float_bufferInOutU[IX2(i + 1, j)] - float_bufferInOutU[IX2(i - 1, j)] + float_bufferInOutV[IX2(i, j + 1)] - float_bufferInOutV[IX2(i, j - 1)]);
    float_bufferInOutU0[IX2(i, j)] = 0;

	//CALL BND ON CPU

}