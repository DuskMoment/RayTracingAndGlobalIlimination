

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
	typedef struct a3_FluidGrid
	{
		//the ammout of cells for each portion of the gird x,y,z
		a3i32 gridCountX;
		a3i32 gridCountY;
		a3i32 gridCountZ;

		//the width height and depth of each grid
		a3i32 gridSize;

		//velocitys of the fluids
	
		//array of veclotiys final
		a3real*** velocityXFinal;
		a3real*** velocityYFinal;
		a3real*** velocityZFinal;

		//array of velocitys temp 
		a3real*** velocityXTemp;
		a3real*** velocityYTemp;
		a3real*** velocityZTemp;

	}a3_FluidGrid;

	//----------------------

	//init the grid
	a3ret InitFluidGrid(a3_FluidGrid* grid, a3i32 gridSize, a3i32 gridCountX, a3i32 gridCountY, a3i32 gridCountZ);

	//init the velocitys
	a3ret InitVelocity(a3_FluidGrid* fluidGrid);

	a3ret DestroyFluidGrid(a3_FluidGrid* grid);
	
#ifdef __cplusplus
}
#endif	// __cplusplus


#endif // #ifndef __ANIMAL3D_A3DX_HULL_H