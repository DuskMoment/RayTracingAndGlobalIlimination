#version 450

#define IX2(i,j) ((i)+(N+2)*(j)) 
/*
THIS RUNS PER PIXEL --> run this 20 times the gpu
and then also in between each loop use SETBND
*/



//CHANGE THE GRIDSIZE HERE 
const int GRID_LENGHT = 38;
const int GRID_SIZE = (GRID_LENGHT + 2) * (GRID_LENGHT + 2);

layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

layout(std430, binding = 1) buffer bufferInOutPrev {

	float float_bufferInOutPrev[];
};

layout(std430, binding = 2) buffer bufferInOutCurr {

	float float_bufferInOutCur[];
};

uniform float uDiffuseConstant;
uniform float uDeltaTime;


void main()
{
   

    int N = GRID_LENGHT;

    //indexs
    int i = int(gl_GlobalInvocationID.x), j = int(gl_GlobalInvocationID.y), k;

    //diffuse constant
    float a = uDeltaTime * uDiffuseConstant * N * N;

    //Gauss-Seidel relaxation - iterative matrix inversion to solve system of equations
    //find densities which when diffused backwards are the previous density
    //20 is arbirary number to bring us 'close enought' to convergence 
//////for (i = 1; i <= N; i++)
//////        {
//////            for (j = 1; j <= N; j++)
//////            {
//////                a3real adjDiff = (float_bufferInOutCur[IX2(i - 1, j)] + float_bufferInOutCur[IX2(i + 1, j)] + float_bufferInOutCur[IX2(i, j - 1)] + float_bufferInOutCur[IX2(i, j + 1)]);
//////                a3real numerator = x0[IX2(i, j)] + a * adjDiff;
//////                a3real denom = (1 + 4 * a);
//////
//////                x[IX2(i, j)] = numerator / denom;
//////            }
//////        }

     float adjDiff = (float_bufferInOutCur[IX2(i - 1, j)] + float_bufferInOutCur[IX2(i + 1, j)] + float_bufferInOutCur[IX2(i, j - 1)] + float_bufferInOutCur[IX2(i, j + 1)]);
     float numerator = float_bufferInOutPrev[IX2(i, j)] + a * adjDiff;
     float denom = (1 + 4 * a);

     float_bufferInOutCur[IX2(i, j)] = numerator / denom;

    
}
