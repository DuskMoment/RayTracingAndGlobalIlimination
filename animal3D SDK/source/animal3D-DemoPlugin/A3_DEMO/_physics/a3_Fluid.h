

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
#define SWAP(u0, u) ()


	static inline Swap(a3real* x, a3real* y)
	{
		a3real* temp;
		temp = y;
		y = x;
		x = temp;
	}

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
	
#define IX2(i,j) ((i)+(N+2)*(j)) 
	typedef struct a3_FluidGrid
	{
		//x and y compoent for the velocity
		a3real* velocityU;
		a3real* velocityV;
		a3real* prevVelocityU;
		a3real* prevVelocityV;

		//x and y of the desnsity
		a3real* density;
		a3real* prevDensity;

		//(N+2)*(N+2)
		a3i32 size;
		a3real diff;

		//with out boarder
		a3i32 length;

	}a3_FluidGrid;


	//2D Simulation

	a3ret InitFluidGrid(a3_FluidGrid* grid, a3i32 N, a3real diffuseConstant);

	a3ret DestroyFluidGrid(a3_FluidGrid* grid);

	a3ret FluidGirdAddSource(a3i32 N, a3real* x, a3real* s, a3real dt);

	//diffuse
	a3ret FluidGridDiffuse(a3i32 N, a3i32 b, a3real* x, a3real* x0, a3real diff, a3real dt);
	

	//project
	a3ret FluidGridProject(a3i32 N, a3real* u, a3real* v, a3real* p, a3real* div);
	
	//advect
	a3ret FluidGridAdvect(a3i32 N, a3i32 b, a3real* d, a3real* d0, a3real* u, a3real* v, a3real dt);


	
	//advance simulation
	a3ret FluidGridVelStep(a3i32 N, a3real* u, a3real* v, a3real* u0, a3real* v0,
		a3real visc, a3real dt);

	a3ret FluidGridDensStep(a3i32 N, a3real* x, a3real* x0, a3real* u, a3real* v,
		a3real diff, a3real dt);

	a3ret FluidGridSim(a3_FluidGrid* grid, a3i32 N, a3real* u, a3real* v, a3real visc, a3real dt);

	//read simulation
	a3vec2 ReadFluidSimulationVel(a3_FluidGrid* grid, a3i32 x, a3i32 y);
	a3real ReadFluidSimulationDensity(a3_FluidGrid* grid, a3i32 x, a3i32 y);

	//helper 

	//set_bnd
	a3ret FluidGridSetBnd(a3i32 N, a3i32 b, a3real* x);
	

	


#ifdef __cplusplus
}
#endif	// __cplusplus


#endif // #ifndef __ANIMAL3D_A3DX_HULL_H