
#include "../a3_Fluid.h"


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
