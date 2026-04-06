#version 450

layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

layout(std430, binding = 1) buffer densityBuf_out {

	float density_out[];
};


//TODO:WILL
a3ret CopyCurrToPrevGrids(a3_FluidGrid* grid)
{
    for (int i = 0; i < grid->size; i++)
    {
        grid->prevDensity[i] = grid->density[i];
        grid->prevVelocityU[i] = grid->velocityU[i];
        grid->prevVelocityV[i] = grid->velocityV[i];
    }

    return 1;
}

//TODO:TRISTAN
//adds density to the grid
a3ret FluidGridAddSource(a3i32 N, a3real* x, a3real* s, a3real dt)
{
    a3i32 i, size = N;

    for (i = 0; i < size; i++)
    {
        x[i] += dt * s[i];
    }

    return 1;
}

//TODO:ANNABELLE
//fade density over time
a3ret FluidGridFade(a3i32 N, a3real* x, a3real* s, a3real dt)
{
    a3i32 i, size = N;

    for (i = 0; i < size; i++)
    {
        if (x[i] > 1.0)
        {
            x[i] = 1.0;
        }

        if (x[i] <= 0)
        {
            x[i] = (a3real)0.0;
        }
        else
        {
            x[i] -= dt * s[i];
        }
    }

    return 1;
}

//TODO:WILL
//density exchange between neighbors 
a3ret FluidGridDiffuse(a3i32 N, a3i32 b, a3real* x, a3real* x0, a3real diff, a3real dt)
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
                a3real adjDiff = (x[IX2(i - 1, j)] + x[IX2(i + 1, j)] + x[IX2(i, j - 1)] + x[IX2(i, j + 1)]);
                a3real numerator = x0[IX2(i, j)] + a * adjDiff;
                a3real denom = (1 + 4 * a);

                x[IX2(i, j)] = numerator / denom;
            }
        }

        FluidGridSetBnd(N, b, x);
    }

    return 1;
}

//TODO: TRISTAN
//forces velocity to be mass conserving, forces the flow to have more swirls
a3ret FluidGridProject(a3i32 N, a3real* u, a3real* v, a3real* p, a3real* div)
{
    {
        a3i32 i, j, k;
        a3real h;
        h = (a3real)1.0 / N;

        a3real half = 0.5;


        //add third 
        //solve divergence
        for (i = 1; i <= N; i++)
        {
            for (j = 1; j <= N; j++)
            {

                div[IX2(i, j)] = -half * h * (u[IX2(i + 1, j)] - u[IX2(i - 1, j)] +
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
                        p[IX2(i, j - 1)] + p[IX2(i, j + 1)]) / (a3real)4;
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
                u[IX2(i, j)] -= half * (p[IX2(i + 1, j)] - p[IX2(i - 1, j)]) / h;
                v[IX2(i, j)] -= half * (p[IX2(i, j + 1)] - p[IX2(i, j - 1)]) / h;
            }
        }
        FluidGridSetBnd(N, 1, u); FluidGridSetBnd(N, 2, v);
    }

    return 1;
}

//TODO:TRISTAIN
//forces density to follow velocity field
//Semi-Lagrangian, treats the center of each cell as a particle
a3ret FluidGridAdvect(a3i32 N, a3i32 b, a3real* d, a3real* d0, a3real* u, a3real* v, a3real dt)
{
    a3i32 i, j, i0, j0, i1, j1;
    a3real x, y, s0, t0, s1, t1, dt0;

    dt0 = dt * N;

    a3real half = 0.5;
    for (i = 1; i <= N; i++)
    {
        for (j = 1; j <= N; j++)
        {
            //index before timestep
            x = i - dt0 * u[IX2(i, j)]; //horizontal
            y = j - dt0 * v[IX2(i, j)]; //vertical

            //clamps edge cases
            if (x < half)
                x = half;

            if (x > (a3real)N + half)
                x = (a3real)N + half;

            //current i and one over i
            i0 = (a3i32)x; 
            i1 = i0 + 1;

            //clamps edge case
            if (y < half)
                y = half;

            if (y > N + half)
                y = N + half;

            //current and down 1 j
            j0 = (a3i32)y;
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


    return 1;
}
//THIS WILL BE CPU SIDE
a3ret FluidGridVelStep(a3i32 N, a3real* u, a3real* v, a3real* u0, a3real* v0, a3real visc, a3real dt)
{
    FluidGridAddSource(N, u, u0, dt); 
    FluidGridAddSource(N, v, v0, dt);
    Swap(u0, u, (N + 2)*(N+2)); 
    FluidGridDiffuse(N, 1, u, u0, visc, dt);
    Swap(v0, v, (N + 2) * (N + 2));
    FluidGridDiffuse(N, 2, v, v0, visc, dt);
    FluidGridProject(N, u, v, u0, v0);
    Swap(u0, u, (N + 2) * (N + 2));
    Swap(v0, v, (N + 2) * (N + 2));
    FluidGridAdvect(N, 1, u, u0, u0, v0, dt); 
    FluidGridAdvect(N, 2, v, v0, u0, v0, dt);
    FluidGridProject(N, u, v, u0, v0);

    return 1;
}
//CPU SIDE
a3ret FluidGridDensStep(a3i32 N, a3real* x, a3real* x0, a3real* u, a3real* v, a3real diff, a3real dt)
{
    FluidGridAddSource(N, x, x0, dt);
    Swap(x0, x, (N + 2) * (N + 2));
    FluidGridDiffuse(N, 0, x, x0, diff, dt);
    Swap(x0, x, (N + 2) * (N + 2));
    FluidGridAdvect(N, 0, x, x0, u, v, dt);

    return 1;
}
//THIS WILL BE CPU SIDE
a3ret FluidGridSim(a3_FluidGrid* grid, a3i32 N, a3real* u, a3real* v, a3real visc, a3real dt)
{
    a3real* densPrev, * uPrev, * vPrev;
    //previous frame
    densPrev = grid->prevDensity;
    uPrev = grid->prevVelocityU;
    vPrev = grid->prevVelocityV;

    FluidGridVelStep(N, u, v, uPrev, vPrev, visc, dt);
    FluidGridDensStep(N, grid->density, densPrev, u, v, grid->diff, dt);

    return 1;
}

//TODO:WILL
a3ret FluidGridSetBnd(a3i32 N, a3i32 b, a3real* x)
{
    a3i32 i;

    /*
    * loop iterates of the dashes for calculation of bounds
    * 0----0
    * |	   |
    * |	   |
    * |    |
    * 0----0
    */
    for (i = 1; i <= N; i++) {

        a3real leftVert = x[IX2(1, i)];
        a3real rightVert = x[IX2(N, i)];

        a3real topHori = x[IX2(i, 1)];
        a3real bottomHori = x[IX2(i, N)];


        x[IX2(0, i)] = b == 1 ? ( -(a3real)1 * leftVert) : leftVert;

        x[IX2(N + 1, i)] = b == 1 ? (-(a3real)1 * rightVert): rightVert;

        x[IX2(i, 0)] = b == 2 ? (-(a3real)1 * topHori) : topHori;

        x[IX2(i, N + 1)] = b == 2 ? (-(a3real)1 * bottomHori) : bottomHori;

        //add third dimention here 
    }

    //this handles the corners takes the average using adjacecys 

    a3real half = 0.5;
    //top left
    x[IX2(0, 0)] = half * (x[IX2(1, 0)] + x[IX2(0, 1)]);

    //bottom left
    x[IX2(0, N + 1)] = half * (x[IX2(1, N + 1)] + x[IX2(0, N)]);

    //top right
    x[IX2(N + 1, 0)] = half * (x[IX2(N, 0)] + x[IX2(N + 1, 1)]);

    //bottom right
    x[IX2(N + 1, N + 1)] = half * (x[IX2(N, N + 1)] + x[IX2(N + 1, N)]);

    //add third dimention here 

    return 1;
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