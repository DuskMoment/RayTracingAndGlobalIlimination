#ifndef __ANIMAL3D_A3DX_FLUID_GPU_H
#define __ANIMAL3D_A3DX_FLUID_GPU_H

#include "animal3D-A3DG/animal3D-A3DG.h"

#include <gl/glew.h>
#include <animal3D-A3DM/a3math/a3vector.h>

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
		a3real 
			viscocityConstant, 
			diffuseConstant;
		a3ui32
			gridLength;
	}a3_FluidGrid_GPU;

	a3ret InitFluidGrid_GPU(a3_FluidGrid_GPU* gridData, a3ui32 gridSize);

	a3ret FluidGridAddSourceFromGrid_GPU(a3_FluidGrid_GPU* gridData, a3_ShaderProgram* shaderProgram, a3_UniformBuffer* source, a3_UniformBuffer* dest);
	a3ret FluidGridAddSourceFromPoint_GPU(a3_FluidGrid_GPU* gridData, a3_ShaderProgram* shaderProgram, a3_UniformBuffer* source, a3vec2 point, a3ui32 pixelRadius);

	a3ret FluidGridVelStep_GPU(a3_FluidGrid_GPU* gridData, a3_ShaderProgram* shaderPrograms, a3real dt);

	a3ret FluidDensityStep_GPU(a3_FluidGrid_GPU* gridData, a3_ShaderProgram* shaderPrograms, a3real dt);

	a3ret RunFluidSim_GPU(a3_FluidGrid_GPU* gridData, a3_ShaderProgram* shaderPrograms, a3real dt);

	a3ret FreeFluidGrid_GPU(a3_FluidGrid_GPU* gridData);

#ifdef __cplusplus
}
#endif	// __cplusplus


#endif // #ifndef __ANIMAL3D_A3DX_FLUID_GPU_H