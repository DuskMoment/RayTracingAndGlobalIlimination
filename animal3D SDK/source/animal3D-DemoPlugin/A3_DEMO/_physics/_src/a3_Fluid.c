
#include "../a3_Fluid.h"
#include <math.h>


//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------

a3ret InitFluidCube(a3_FluidCube* cube, a3real diffusion, a3real viscosity, a3i32 size, a3real dt)
{
	cube = malloc(sizeof(a3_FluidCube*));

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
    free(cube->s);
    free(cube->density);

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
