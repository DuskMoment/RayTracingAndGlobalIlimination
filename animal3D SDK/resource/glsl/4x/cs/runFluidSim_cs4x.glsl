#version 450

layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

layout(std430, binding = 1) buffer densityBuf_out {

	float density_out[];
};
const int GRID_SIZE = 800 * 800;
#define IX2(i,j) ((i)+(N+2)*(j)) 


//TODO:WILL
void FluidGridSetBnd(int N, int b, float x[GRID_SIZE])
{
    int i;

    /*
    * loop iterates of the dashes for calculation of bounds
    * 0----0
    * |	   |
    * |	   |
    * |    |
    * 0----0
    */
    for (i = 1; i <= N; i++) {

        float leftVert = x[IX2(1, i)];
        float rightVert = x[IX2(N, i)];

        float topHori = x[IX2(i, 1)];
        float bottomHori = x[IX2(i, N)];


        x[IX2(0, i)] = b == 1 ? ( -1.0 * leftVert) : leftVert;

        x[IX2(N + 1, i)] = b == 1 ? (-1.0 * rightVert): rightVert;

        x[IX2(i, 0)] = b == 2 ? (-1.0 * topHori) : topHori;

        x[IX2(i, N + 1)] = b == 2 ? (-1.0 * bottomHori) : bottomHori;

        //add third dimention here 
    }

    //this handles the corners takes the average using adjacecys 

    float h = 0.5;
    //top left
    x[IX2(0, 0)] = h * (x[IX2(1, 0)] + x[IX2(0, 1)]);

    //bottom left
    x[IX2(0, N + 1)] = h * (x[IX2(1, N + 1)] + x[IX2(0, N)]);

    //top right
    x[IX2(N + 1, 0)] = h * (x[IX2(N, 0)] + x[IX2(N + 1, 1)]);

    //bottom right
    x[IX2(N + 1, N + 1)] = h * (x[IX2(N, N + 1)] + x[IX2(N + 1, N)]);

    //add third dimention here 
}

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
//a3ret FluidGridFade(a3i32 N, a3real* x, a3real* s, a3real dt)
//{
//    a3i32 i, size = N;
//
//    for (i = 0; i < size; i++)
//    {
//        if (x[i] > 1.0)
//        {
//            x[i] = 1.0;
//        }
//
//        if (x[i] <= 0)
//        {
//            x[i] = (a3real)0.0;
//        }
//        else
//        {
//            x[i] -= dt * s[i];
//        }
//    }
//
//    return 1;
//}

//density exchange between neighbors 
void FluidGridDiffuse(int N, int b, float x[GRID_SIZE], float x0[GRID_SIZE], float diff, float dt)
{
    //indexs
    int i, j, k;

    //diffuse constant
    float a = dt * diff * N * N;

    //Gauss-Seidel relaxation - iterative matrix inversion to solve system of equations
    //find densities which when diffused backwards are the previous density
    //20 is arbirary number to bring us 'close enought' to convergence 
    for (k = 0; k < 20; k++)
    {
        for (i = 1; i <= N; i++)
        {
            for (j = 1; j <= N; j++)
            {
                float adjDiff = (x[IX2(i - 1, j)] + x[IX2(i + 1, j)] + x[IX2(i, j - 1)] + x[IX2(i, j + 1)]);
                float numerator = x0[IX2(i, j)] + a * adjDiff;
                float denom = (1 + 4 * a);

                x[IX2(i, j)] = numerator / denom;
            }
        }

        FluidGridSetBnd(N, b, x);
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
            if (x < 0.5)
                x = 0.5;

            if (x > float(N) + 0.5)
                x = float(N) + 0.5;

            //current i and one over i
            i0 = int(x); 
            i1 = i0 + 1;

            //clamps edge case
            if (y < 0.5)
                y = 0.5;

            if (y > N + 0.5)
                y = N + 0.5;

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