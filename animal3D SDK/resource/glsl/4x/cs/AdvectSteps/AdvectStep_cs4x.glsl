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

layout(std430, binding = 1) buffer bufferInOutU {

	float float_bufferInOutU[];
};

layout(std430, binding = 2) buffer bufferInOutV {

	float float_bufferInOutV[];
};

layout(std430, binding = 3) buffer bufferInOutD {

	float float_bufferInOutD[];
};

layout(std430, binding = 4) buffer bufferInOutD0 {

	float float_bufferInOutD0[];
};

uniform float uDt;
uniform float uDir;

void main()
{
    float b = uDir;
    float dt = uDt;

    int N = GRID_LENGHT;

	int i =  int(gl_GlobalInvocationID.x) + 1 , j =  int(gl_GlobalInvocationID.y) + 1 
    , i0, j0, i1, j1;
    float x, y, s0, t0, s1, t1, dt0;

    dt0 = dt * N;

     //index before timestep
     x = i - dt0 * float_bufferInOutU[IX2(i, j)]; //horizontal
     y = j - dt0 * float_bufferInOutV[IX2(i, j)]; //vertical

     //clamps edge cases
     //x = clamp(x, 0.5, float(N) + 0.5);
     if (x < 0.5)
         x = 0.5;

      if (x > float(N) + 0.5)
        x = float(N)+ 0.5;


     //current i and one over i
     i0 = int(x); 
     i1 = i0 + 1;

     //clamps edge case
      if (y < 0.5)
        y = 0.5;

       if (y > float(N) + 0.5)
          y = float(N) + 0.5;

     //current and down 1 j
     j0 = int(y);
     j1 = j0 + 1;

     //interpolation weights
     s1 = x - i0; 
     s0 = 1 - s1;
     t1 = y - j0; 
     t0 = 1 - t1;

     //neighbor interpolation for final density
     float_bufferInOutD[IX2(i, j)] = s0 * (t0 * float_bufferInOutD0[IX2(i0, j0)] + t1 * float_bufferInOutD0[IX2(i0, j1)]) +
         s1 * (t0 * float_bufferInOutD0[IX2(i1, j0)] + t1 * float_bufferInOutD0[IX2(i1, j1)]);  
         


}