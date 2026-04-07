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


//forces velocity to be mass conserving, forces the flow to have more swirls
void FluidGridProject(int N, float u[GRID_SIZE], float v[GRID_SIZE], float p[GRID_SIZE], float div[GRID_SIZE])
{
    {
        int i, j, k;
        float h;
        h = 1.0 / N;


        //add third 
        //solve divergence
        for (i = 1; i <= N; i++)
        {
            for (j = 1; j <= N; j++)
            {

                div[IX2(i, j)] = -0.5 * h * (u[IX2(i + 1, j)] - u[IX2(i - 1, j)] +
                    v[IX2(i, j + 1)] - v[IX2(i, j - 1)]);
                p[IX2(i, j)] = 0;
            }
        }

        //use gauss-siedel to solve poisson equation to get height field/gradient
        FluidGridSetBnd(N, 0, div); FluidGridSetBnd(N, 0, p);
        for (k = 0; k < 20; k++) 
        {
            //add third
            for (i = 1; i <= N; i++) 
            {
                for (j = 1; j <= N; j++) 
                {
                    p[IX2(i, j)] = (div[IX2(i, j)] + p[IX2(i - 1, j)] + p[IX2(i + 1, j)] +
                        p[IX2(i, j - 1)] + p[IX2(i, j + 1)]) / 4.0;
                }
            }
            FluidGridSetBnd(N, 0, p);
        }

        //add third
        //Hodge decomposition = mass conserving field + gradient field
        //Mass conserving velocity = velocity - gradient field
        for (i = 1; i <= N; i++) 
        {
            for (j = 1; j <= N; j++) 
            {
                u[IX2(i, j)] -= 0.5 * (p[IX2(i + 1, j)] - p[IX2(i - 1, j)]) / h;
                v[IX2(i, j)] -= 0.5 * (p[IX2(i, j + 1)] - p[IX2(i, j - 1)]) / h;
            }
        }
        FluidGridSetBnd(N, 1, u); FluidGridSetBnd(N, 2, v);
    }
}

//forces density to follow velocity field
//Semi-Lagrangian, treats the center of each cell as a particle
void FluidGridAdvect(int N, int b, float d[GRID_SIZE], float d0[GRID_SIZE], float u[GRID_SIZE], float v[GRID_SIZE], float dt)
{
    int i, j, i0, j0, i1, j1;
    float x, y, s0, t0, s1, t1, dt0;

    dt0 = dt * N;

    for (i = 1; i <= N; i++)
    {
        for (j = 1; j <= N; j++)
        {
            //index before timestep
            x = i - dt0 * u[IX2(i, j)]; //horizontal
            y = j - dt0 * v[IX2(i, j)]; //vertical

            //clamps edge cases
            x = clamp(x, 0.5, float(N) + 0.5);
            if (x < 0.5)
                x = 0.5;

            //current i and one over i
            i0 = int(x); 
            i1 = i0 + 1;

            //clamps edge case
            y = clamp(y, 0.5, float(N) + 0.5);
        

            //current and down 1 j
            j0 = int(y);
            j1 = j0 + 1;

            //interpolation weights
            s1 = x - i0; 
            s0 = 1 - s1;
            t1 = y - j0; 
            t0 = 1 - t1;

            //neighbor interpolation for final density
            d[IX2(i, j)] = s0 * (t0 * d0[IX2(i0, j0)] + t1 * d0[IX2(i0, j1)]) +
                s1 * (t0 * d0[IX2(i1, j0)] + t1 * d0[IX2(i1, j1)]);
        }
    }

    FluidGridSetBnd(N, b, d);
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