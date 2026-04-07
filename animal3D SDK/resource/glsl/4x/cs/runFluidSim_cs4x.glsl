#version 450

layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

layout(std430, binding = 1) buffer densityBuf_out {

	float density_out[];
};
const int GRID_SIZE = 800 * 800;
#define IX2(i,j) ((i)+(N+2)*(j));




void CopyCurrToPrevGrids(float prevDes[GRID_SIZE], float prevVelU[GRID_SIZE], float prevVelV[GRID_SIZE]
, float des[GRID_SIZE], float velU[GRID_SIZE], float velV[GRID_SIZE])
{
    for (int i = 0; i < GRID_SIZE; i++)
    {
        prevDes[i] = des[i];
        prevVelU[i] = velU[i]; 
        prevVelV[i] = velV[i];
    }
}

//adds density to the grid
void FluidGridAddSource(int N, float x[GRID_SIZE], float s[GRID_SIZE], float dt)
{
    int i, size = N;

    for (i = 0; i < size; i++)
    {
        x[i] += dt * s[i];
    }
}

//TODO:ANNABELLE
//fade density over time
void FluidGridFade(int N, float x[GRID_SIZE], float s[GRID_SIZE], float dt)
{
    float v;

    for (int i = 0; i < N; i++)
    {
        x[i] = max(0.0, min(1.0, x[i]));
        x[i] -= dt * s[i];
    }
}



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