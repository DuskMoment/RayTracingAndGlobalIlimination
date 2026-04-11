
#include "../a3_Fluid.h"
#include <math.h>


//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------

a3ret InitFluidCube(a3_FluidCube* cube, a3real diffusion, a3real viscosity, a3i32 size, a3real dt)
{
	a3i32 N = size;

    cube->gridSize = size;
    cube->dt = dt;
    cube->diff = diffusion;
    cube->visc = viscosity;

    //create the desity state arrays
    cube->s = calloc(N * N * N, sizeof(a3real));
    cube->density = calloc(N * N * N, sizeof(a3real));

    //create the velocity state arrays
    cube->vX = calloc(N * N * N, sizeof(a3real));
    cube->vY = calloc(N * N * N, sizeof(a3real));
    cube->vZ = calloc(N * N * N, sizeof(a3real));

    cube->vX0 = calloc(N * N * N, sizeof(a3real));
    cube->vY0 = calloc(N * N * N, sizeof(a3real));
    cube->vZ0 = calloc(N * N * N, sizeof(a3real));

	 
	return 1;
}

a3ret DestroyFluidCube(a3_FluidCube* cube)
{
    if (cube == NULL) { return 0; }
    free(cube->s);
    free(cube->density);

    cube->gridSize = 0;
    cube->dt = 0;
    cube->diff = 0;
    cube->visc = 0;


    free(cube->vX);
    free(cube->vY);
    free(cube->vZ);
    free(cube->vX0);
    free(cube->vY0);
    free(cube->vZ0);
   
    free(cube);

    return 1;
}

a3ret FluidCubeAddVelocity(a3_FluidCube* cube, a3vec3 gidPos, a3vec3 vel)
{
    if (cube == NULL)
    {
        return -1;
    }

    //used for the macro
    a3i32 N = cube->gridSize;
    //get the grid index
    a3i32 index = IX((a3i32)gidPos.x, (a3i32)gidPos.y, (a3i32)gidPos.z);


    //add to the current velocity
    cube->vX[index] += vel.x;
    cube->vY[index] += vel.y;
    cube->vZ[index] += vel.z;

    return 1;
}

//this what is called a liner back trace?  https://www.reddit.com/r/computergraphics/comments/2s86gi/need_help_understanding_advection_code_in_jos/
a3ret FluidAdvect(a3i32 b, a3real* d, a3real* d0, a3real* velocX, a3real* velocY, a3real* velocZ, a3real dt, a3i32 N)
{
    //velocirtyd current/ prev for each axis 
    a3real i0, i1, j0, j1, k0, k1;

    //delta time multiplyed by delta time
    a3real dtx = dt * (N - 2);
    a3real dty = dt * (N - 2);
    a3real dtz = dt * (N - 2);

    //
    a3real s0, s1, t0, t1, u0, u1;
    a3real tmp1, tmp2, tmp3, x, y, z;

    a3real Nfloat = (a3real)N;
    a3real ifloat, jfloat, kfloat;
    a3i32 i, j, k;

    for (k = 1, kfloat = 1; k < N - 1; k++, kfloat++) {
        for (j = 1, jfloat = 1; j < N - 1; j++, jfloat++) {
            for (i = 1, ifloat = 1; i < N - 1; i++, ifloat++) {
                tmp1 = dtx * velocX[IX(i, j, k)]; //take the veolcity at ay cell and move it ito the future x 
                tmp2 = dty * velocY[IX(i, j, k)]; // take the veolcity at ay cell and move it ito the future x
                tmp3 = dtz * velocZ[IX(i, j, k)]; // take the veolcity at ay cell and move it ito the future x
                x = ifloat - tmp1; //sub tract that vellcity from the loop counter? x
                y = jfloat - tmp2; //sub tract that vellcity from the loop counter? y
                z = kfloat - tmp3; //sub tract that vellcity from the loop counter? z

                //predicts the new cell index
                if (x < 0.5f) x = 0.5f;

                if (x > Nfloat + 0.5f) x = Nfloat + 0.5f;
               
                i0 = floorf(x);
                i1 = i0 + 1.0f;

                if (y < 0.5f) y = 0.5f;

                if (y > Nfloat + 0.5f) y = Nfloat + 0.5f;

                j0 = floorf(y);
                j1 = j0 + 1.0f;

                if (z < 0.5f) z = 0.5f;
                if (z > Nfloat + 0.5f) z = Nfloat + 0.5f;

                k0 = floorf(z);
                k1 = k0 + 1.0f;

                s1 = x - i0;
                s0 = 1.0f - s1;
                t1 = y - j0;
                t0 = 1.0f - t1;
                u1 = z - k0;
                u0 = 1.0f - u1;

                //set new indexs
                a3i32 i0i = (a3i32)i0;
                a3i32 i1i = (a3i32)i1;
                a3i32 j0i = (a3i32)j0;
                a3i32 j1i = (a3i32)j1;
                a3i32 k0i = (a3i32)k0;
                a3i32 k1i = (a3i32)k1;

                //averagre it all together 
                d[IX(i, j, k)] =

                    s0 * (t0 * (u0 * d0[IX(i0i, j0i, k0i)]
                        + u1 * d0[IX(i0i, j0i, k1i)])
                        + (t1 * (u0 * d0[IX(i0i, j1i, k0i)]
                            + u1 * d0[IX(i0i, j1i, k1i)])))
                    + s1 * (t0 * (u0 * d0[IX(i1i, j0i, k0i)]
                        + u1 * d0[IX(i1i, j0i, k1i)])
                        + (t1 * (u0 * d0[IX(i1i, j1i, k0i)]
                            + u1 * d0[IX(i1i, j1i, k1i)])));
            }
        }
    }
    FluidSetBoundry(b, d, N);

    return -1;
}

void FluidSetBoundry(a3i32 b, a3real* x, a3i32 N)
{
   //if at a conner peice then flip the velocity based on the inputed axis  along the edge of the cube
    //z axis
    for (int j = 1; j < N - 1; j++) {
        for (int i = 1; i < N - 1; i++) {
            x[IX(i, j, 0)] = b == 3 ? -x[IX(i, j, 1)] : x[IX(i, j, 1)];
            x[IX(i, j, N - 1)] = b == 3 ? -x[IX(i, j, N - 2)] : x[IX(i, j, N - 2)];
        }
    }
    //y axis
    for (int k = 1; k < N - 1; k++) {
        for (int i = 1; i < N - 1; i++) {
            x[IX(i, 0, k)] = b == 2 ? -x[IX(i, 1, k)] : x[IX(i, 1, k)];
            x[IX(i, N - 1, k)] = b == 2 ? -x[IX(i, N - 2, k)] : x[IX(i, N - 2, k)];
        }
    }
    //x axis
    for (int k = 1; k < N - 1; k++) {
        for (int j = 1; j < N - 1; j++) {
            x[IX(0, j, k)] = b == 1 ? -x[IX(1, j, k)] : x[IX(1, j, k)];
            x[IX(N - 1, j, k)] = b == 1 ? -x[IX(N - 2, j, k)] : x[IX(N - 2, j, k)];
        }
    }

    ///handels coner cases
    x[IX(0, 0, 0)] = 0.33f * (x[IX(1, 0, 0)]
        + x[IX(0, 1, 0)]
        + x[IX(0, 0, 1)]);

    x[IX(0, N - 1, 0)] = 0.33f * (x[IX(1, N - 1, 0)]
        + x[IX(0, N - 2, 0)]
        + x[IX(0, N - 1, 1)]);

    x[IX(0, 0, N - 1)] = 0.33f * (x[IX(1, 0, N - 1)]
        + x[IX(0, 1, N - 1)]
        + x[IX(0, 0, N)]);

    x[IX(0, N - 1, N - 1)] = 0.33f * (x[IX(1, N - 1, N - 1)]
        + x[IX(0, N - 2, N - 1)]
        + x[IX(0, N - 1, N - 2)]);

    x[IX(N - 1, 0, 0)] = 0.33f * (x[IX(N - 2, 0, 0)]
        + x[IX(N - 1, 1, 0)]
        + x[IX(N - 1, 0, 1)]);

    x[IX(N - 1, N - 1, 0)] = 0.33f * (x[IX(N - 2, N - 1, 0)]
        + x[IX(N - 1, N - 2, 0)]
        + x[IX(N - 1, N - 1, 1)]);

    x[IX(N - 1, 0, N - 1)] = 0.33f * (x[IX(N - 2, 0, N - 1)]
        + x[IX(N - 1, 1, N - 1)]
        + x[IX(N - 1, 0, N - 2)]);

    x[IX(N - 1, N - 1, N - 1)] = 0.33f * (x[IX(N - 2, N - 1, N - 1)]
        + x[IX(N - 1, N - 2, N - 1)]
        + x[IX(N - 1, N - 1, N - 2)]);
}



//-----------------------------2D code-------------------------------------------

a3ret InitFluidGrid(a3_FluidGrid* grid, a3i32 N, a3real diffuseConstant, a3real fadeConstant)
{

    if (grid == NULL)
    {
        return -1;
    }
    grid->size = (N + 2) * (N + 2);

    grid->length = N;

    grid->diff = diffuseConstant;

    grid->fade = fadeConstant;

    a3i32 allocSize = grid->size;

    a3i32 totalSize = allocSize * sizeof(a3real) * 6;
    void* chunck = malloc(totalSize);

    //check to see if this is correct
    //grid->density = calloc(allocSize, sizeof(a3real));

    grid->density = (a3real*)chunck;

    for (int i = 0; i < grid->size; i++)
    {
        grid->density[i] = 0;
    }

    //grid->prevDensity = calloc(allocSize, sizeof(a3real));
    grid->prevDensity = (a3real*)chunck + (allocSize);
    for (int i = 0; i < grid->size; i++)
    {
        grid->prevDensity[i] = 0;
    }

    //grid->velocityU = calloc(allocSize, sizeof(a3real));
    grid->velocityU = (a3real*)chunck + (allocSize * 2);
    for (int i = 0; i < grid->size; i++)
    {
        grid->velocityU[i] = 0;
    }

    //grid->velocityV = calloc(allocSize, sizeof(a3real));
    grid->velocityV = (a3real*)chunck + (allocSize * 3);
    for (int i = 0; i < grid->size; i++)
    {
        grid->velocityV[i] = 0;
    }

    //grid->prevVelocityU = calloc(allocSize, sizeof(a3real));
    grid->prevVelocityU = (a3real*)chunck + (allocSize * 4);
    for (int i = 0; i < grid->size; i++)
    {
        grid->prevVelocityU[i] = 0;
    }

    //grid->prevVelocityV = calloc(allocSize, sizeof(a3real));
    grid->prevVelocityV = (a3real*)chunck + (allocSize * 5);
    for (int i = 0; i < grid->size; i++)
    {
        grid->prevVelocityV[i] = 0;
    }


    return 1;
}

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

a3ret DestroyFluidGrid(a3_FluidGrid* grid)
{

    free(grid->density);
    free(grid->prevDensity);

    free(grid->velocityU);
    free(grid->velocityV);
    free(grid->prevVelocityU);
    free(grid->prevVelocityV);

    grid->size = 0;
    grid->diff = 0;
    grid->fade = 0;

    free(grid);

    return 1;
}

//adds density to the grid N, u, u0, dt
a3ret FluidGridAddSource(a3i32 N, a3real* x, a3real* s, a3real dt)
{
    a3i32 i, size = N;

    for (i = 0; i < size; i++)
    {
        x[i] += dt * s[i];
    }

    return 1;
}

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

//forces velocity to be mass conserving, forces the flow to have more swirls (N, u, v, u0, v0)
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

a3ret FluidGridDensStep(a3i32 N, a3real* x, a3real* x0, a3real* u, a3real* v, a3real diff, a3real dt)
{
    FluidGridAddSource(N, x, x0, dt);
    Swap(x0, x, (N + 2) * (N + 2));
    FluidGridDiffuse(N, 0, x, x0, diff, dt);
    Swap(x0, x, (N + 2) * (N + 2));
    FluidGridAdvect(N, 0, x, x0, u, v, dt);

    return 1;
}

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

//returns a copy
a3vec2 ReadFluidSimulationVel(a3_FluidGrid* grid, a3i32 x, a3i32 y)
{
    a3i32 N = grid->length;

    a3real tmpX = grid->velocityU[IX2(x, y)];
    a3real tmpY = grid->velocityV[IX2(x, y)];

    a3vec2 out;

    out.x = tmpX;
    out.y = tmpY;

    return out;
}

//returns a copy
a3real ReadFluidSimulationDensity(a3_FluidGrid* grid, a3i32 x, a3i32 y)
{
    a3i32 N = grid->size;

    return grid->density[IX2(x,y)];
}

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
