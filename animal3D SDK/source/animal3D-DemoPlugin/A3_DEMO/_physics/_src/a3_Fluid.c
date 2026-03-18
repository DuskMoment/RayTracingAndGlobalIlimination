
#include "../a3_Fluid.h"


//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------

a3ret InitFluidGrid(a3_FluidGrid* grid, a3i32 gridSize, a3i32 gridCountX, a3i32 gridCountY, a3i32 gridCountZ)
{
	
	grid = malloc(sizeof(a3_FluidGrid));


	if (grid != NULL)
	{
		grid->gridCountX = gridCountX;
		grid->gridCountY = gridCountY;
		grid->gridCountZ = gridCountZ;

		grid->gridSize = gridSize;
		
		//good data
		return  1;
	}

	return -1;
}

//TODO CHANGE THIS TO THE RIGHT CONSTANT FOR SAFTY -- this method is also extreamly slow...
a3ret InitVelocity(a3_FluidGrid* fluidGrid)
{
	//failed
	if (fluidGrid == NULL)
	{
		return -1;
	}

	a3i32 x = fluidGrid->gridCountX;
	a3i32 y = fluidGrid->gridCountY;
	a3i32 z = fluidGrid->gridCountZ;


	//ALLOCATE THE X  + 1 GIRD for final
	a3real*** array = (a3real***)malloc(x + 1 * sizeof(a3real**));

	for (int i = 0; i < x + 1; i++) {

		array[i] = (a3real**)malloc(y * sizeof(a3real*));

		for (int j = 0; j < y; j++) {

			array[i][j] = (a3real*)malloc(z * sizeof(a3real3));
		}

	}
	fluidGrid->velocityXFinal = array;

	//ALLOCATE FOR THE X + 1 GRIND for temp
	array = (a3real***)malloc(x + 1 * sizeof(a3real**));

	for (int i = 0; i < x + 1; i++) {

		array[i] = (a3real**)malloc(y * sizeof(a3real*));

		for (int j = 0; j < y; j++) {

			array[i][j] = (a3real*)malloc(z * sizeof(a3real3));
		}

	}
	fluidGrid->velocityXTemp = array;

	//ALLOCATE FOR THE Y + 1 GRIND for final
	array = (a3real***)malloc(x * sizeof(a3real**));

	for (int i = 0; i < x; i++) {

		array[i] = (a3real**)malloc(y + 1 * sizeof(a3real*));

		for (int j = 0; j < y + 1; j++) {

			array[i][j] = (a3real*)malloc(z * sizeof(a3real3));
		}

	}
	fluidGrid->velocityYFinal = array;

	//ALLOCATE FOR THE Y + 1 GRIND for temp
	array = (a3real***)malloc(x  * sizeof(a3real**));

	for (int i = 0; i < x; i++) {

		array[i] = (a3real**)malloc(y + 1 * sizeof(a3real*));

		for (int j = 0; j < y + 1; j++) {

			array[i][j] = (a3real*)malloc(z * sizeof(a3real3));
		}

	}
	fluidGrid->velocityYTemp = array;


	//ALLOCATE FOR THE Z + 1 GRIND for final
	array = (a3real***)malloc(x * sizeof(a3real**));

	for (int i = 0; i < x; i++) {

		array[i] = (a3real**)malloc(y  * sizeof(a3real*));

		for (int j = 0; j < y; j++) {

			array[i][j] = (a3real*)malloc(z + 1 * sizeof(a3real3));
		}

	}
	fluidGrid->velocityZFinal = array;

	//ALLOCATE FOR THE Z + 1 GRIND for temp
	array = (a3real***)malloc(x * sizeof(a3real**));

	for (int i = 0; i < x; i++) {

		array[i] = (a3real**)malloc(y * sizeof(a3real*));

		for (int j = 0; j < y; j++) {

			array[i][j] = (a3real*)malloc(z + 1 * sizeof(a3real3));
		}

	}
	fluidGrid->velocityZTemp = array;
	
	return -1;
}
