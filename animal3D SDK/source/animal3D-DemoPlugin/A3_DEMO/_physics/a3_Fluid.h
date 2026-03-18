

#ifndef __ANIMAL3D_A3DX_FLUID_H
#define __ANIMAL3D_A3DX_FLUID_H


#include "animal3D-A3DM/animal3D-A3DM.h"


#ifdef __cplusplus
extern "C"
{
#endif	// __cplusplus
	typedef struct FluidGrid
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
		a3real*** velocityYFinal;

		//array of velocitys temp 
		a3real*** velocityXTemp;
		a3real*** velocityYTemp;
		a3real*** velocityZTemp;

	};
#ifdef __cplusplus
}
#endif	// __cplusplus


#endif // #ifndef __ANIMAL3D_A3DX_HULL_H