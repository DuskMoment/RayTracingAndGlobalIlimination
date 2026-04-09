#ifndef __ANIMAL3D_A3DX_FLUID_GPU_H
#define __ANIMAL3D_A3DX_FLUID_GPU_H

#include "animal3D-A3DM/animal3D-A3DM.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "animal3D/animal3D.h"
#include "animal3D-A3DG/animal3D-A3DG.h"

#ifdef __cplusplus
extern "C"
{
#endif	// __cplusplus

	typedef struct a3_FluidGrid_GPU {
		a3_UniformBuffer
			densityBuffer[1],
			prevDensityBuffer[1],
			velocityBufferU[1],
			prevVelocityBufferU[1],
			velocityBufferV[1],
			prevVelocityBufferV[1];
		void
			*densityBufferData,
			*prevDensityBufferData,
			*velocityBufferUData,
			*prevVelocityBufferUData,
			*velocityBufferVData,
			*prevVelocityBufferVData;
	}a3_FluidGrid_GPU;

	a3ret InitFluidGrid_GPU(a3_FluidGrid_GPU* gridData, a3ui32 gridSize);

	a3ret FluidGridVelStep_GPU(a3_FluidGrid_GPU* gridData, a3ui32 gridSize);

	a3ret FluidDensityStep_GPU(a3_FluidGrid_GPU* gridData, a3ui32 gridSize);

	a3ret RunFluidSim_GPU(a3_FluidGrid_GPU* gridData, a3i32 gridSize, a3real dt);

	a3ret FreeFluidGrid_GPU(a3_FluidGrid_GPU* gridData);

#ifdef __cplusplus
}
#endif	// __cplusplus


#endif // #ifndef __ANIMAL3D_A3DX_FLUID_GPU_H