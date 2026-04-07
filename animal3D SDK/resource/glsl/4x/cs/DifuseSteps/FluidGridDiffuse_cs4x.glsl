#version 450
#define IX2(i,j) ((i)+(N+2)*(j)) 
/*
THIS RUNS PER PIXEL --> run this 20 times the gpu
and then also in between each loop use SETBND
*/



//CHANGE THE GRIDSIZE HERE 
const int GRID_LENGHT = 798;
const int GRID_SIZE = (GRID_LENGHT + 2) * (GRID_LENGHT + 2);

layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

layout(std430, binding = 1) buffer bufferInOutCurrent {

	float float_bufferInOutCur[GRID_SIZE];
};

layout(std430, binding = 2) buffer bufferInOutPrev {

	float float_bufferInOutPrev[GRID_SIZE];
};

uniform int uDirection;
uniform float uDiff;
uniform float uDt;

//density exchange between neighbors 
//N = MAX_GRIDSIZE, b = horizontal or vertical
void FluidGridDiffuse(int N, int b, float x[GRID_SIZE], float x0[GRID_SIZE], float diff, float dt)
{
    //indexs
    uint i = gl_GlobalInvocationID.x, j = gl_GlobalInvocationID.y, k;

    //diffuse constant
    float a = dt * diff * N * N;

    //Gauss-Seidel relaxation - iterative matrix inversion to solve system of equations
    //find densities which when diffused backwards are the previous density
    //20 is arbirary number to bring us 'close enought' to convergence 
//    for (k = 0; k < 20; k++)
//    {
//        for (i = 1; i <= N; i++)
//        {
//            for (j = 1; j <= N; j++)
//            {
//               
//            }
//        }
//    }

     float adjDiff = (x[IX2(i - 1, j)] + x[IX2(i + 1, j)] + x[IX2(i, j - 1)] + x[IX2(i, j + 1)]);
     float numerator = x0[IX2(i, j)] + a * adjDiff;
     float denom = (1 + 4 * a);

     x[IX2(i, j)] = numerator / denom;

}

void main()
{
    //Takes the grid lenght, the direction, current state, previous state, diffuse constant, delta time
    FluidGridDiffuse(GRID_LENGHT, uDirection, float_bufferInOutCur, float_bufferInOutPrev, uDiff, uDt);
    
}
