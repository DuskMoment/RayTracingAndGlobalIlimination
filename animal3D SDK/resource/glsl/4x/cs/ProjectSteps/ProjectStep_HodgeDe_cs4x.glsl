#version 450
#define IX2(i,j) ((i)+(N+2)*(j))

/*THIS RUNS PER PIXEL --> run this 20 times the gpu
and then also in between each loop use SETBND
*/


layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;
//CHANGE THE GRIDSIZE HERE 
const int GRID_LENGHT = 798;
const int GRID_SIZE = (GRID_LENGHT + 2) * (GRID_LENGHT + 2);


//U
layout(std430, binding = 1) buffer bufferInOutU {

	float u[];
};

//V
layout(std430, binding = 2) buffer bufferInOutV {

	float v[];
};

//P
layout(std430, binding = 3) buffer bufferInOutP {

	float p[];
};



void main()
{
	int j = int(gl_GlobalInvocationID.y), 
	i  = int(gl_GlobalInvocationID.x),
	N = GRID_LENGHT;

	 float h;
     h = 1.0 / float(N);

	 //p[IX2(i + 1, j)] =  p[IX2(i + 1, j)];
	 u[IX2(i, j)] -= 0.5 * (p[IX2(i + 1, j)] - p[IX2(i - 1, j)]) / h;
     v[IX2(i, j)] -= 0.5 * (p[IX2(i, j + 1)] - p[IX2(i, j - 1)]) / h;
	
	//CALL BND ON CPU
}