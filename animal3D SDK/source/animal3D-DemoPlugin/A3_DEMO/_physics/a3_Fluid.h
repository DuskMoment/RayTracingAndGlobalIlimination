

#ifndef __ANIMAL3D_A3DX_FLUID_H
#define __ANIMAL3D_A3DX_FLUID_H


#include "animal3D-A3DM/animal3D-A3DM.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#ifdef __cplusplus
extern "C"
{
#endif	// __cplusplus
#define IX(x, y, z) ((x) + (y) * N + (z) * N * N)
	typedef struct a3_FluidCube
	{
	
		a3real visc;
		a3real diff;
		a3real dt;
		//the width height and depth of each grid
		a3i32 gridSize;
	
		//desnity states
		a3real* density;
		a3real* s; 


		//array of veclotiys final
		a3real* vX;
		a3real* vY;
		a3real* vZ;

		//array of velocitys second frame
		a3real* vX0;
		a3real* vY0;
		a3real* vZ0;

	}a3_FluidCube;

	//----------------------

	//init the grid
	a3ret InitFluidCube(a3_FluidCube* cube, a3real diffusion, a3real viscosity, a3i32 size, a3real dt);

	//destroy the cube
	a3ret DestroyFluidCube(a3_FluidCube* cube);

	//add velocity
	a3ret FluidCubeAddVelocity(a3_FluidCube* cube, a3vec3 gidPos, a3vec3 vel);


	//THRE MAIN STEPS for the equation 
	//1. diffues --> disapation of velocitys( spreading out)
	//2. project --> ammount of each fluid in a box needs to remain constant
	//3. advect --> velocity step( directed motion)

	//advect --> will accualy move the velocity field
	a3ret FluidAdvect(a3i32 b, a3real* d, a3real* d0, a3real* velocX, a3real* velocY, a3real* velocZ, a3real dt, a3i32);

	//helpers
	//creates a boundry for the fluid is it does not leak out of this grid
	//x is the arry being acted upon 
	//N  is the size
	//B is the axis
	void FluidSetBoundry(a3i32 b, a3real* x, a3i32 N);
	

#ifdef __cplusplus
}
#endif	// __cplusplus


#endif // #ifndef __ANIMAL3D_A3DX_HULL_H