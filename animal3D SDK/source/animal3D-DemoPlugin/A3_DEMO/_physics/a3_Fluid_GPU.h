#ifndef __ANIMAL3D_A3DX_FLUID_GPU_H
#define __ANIMAL3D_A3DX_FLUID_GPU_H

#include "animal3D-A3DG/animal3D-A3DG.h"

#include <gl/glew.h>
#include <animal3D-A3DM/a3math/a3vector.h>
#include "./A3_DEMO/_a3_scene_utilities/a3_SceneShaderProgram.h"

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
			gridLength,
			diffuse_GS_Loops;
		const a3_ShaderProgram
			* prog_step_difuse,
			* prog_step_advect,
			* prog_step_project_div,
			* prog_step_project_hodgeDe,
			* prog_step_project_poisson,
			* prog_step_setBND,
			* prog_step_add_source_from_grid,
			* prog_step_add_source_from_point,
			* prog_step_swap_buffers,
			* prog_step_init_grid;
	}a3_FluidGrid_GPU;

	a3ret InitFluidGrid_GPU(a3_FluidGrid_GPU* gridData, a3real diffuseK, a3ui32 diffuse_GS_Loops);

	a3ret FluidGridAddSourceFromGrid_GPU(a3_FluidGrid_GPU* gridData, a3_UniformBuffer* dest, a3_UniformBuffer* source, a3real dt);
	a3ret FluidGridAddSourceFromPoint_GPU(a3_FluidGrid_GPU* gridData, a3_UniformBuffer* source, a3vec2 point, a3real vel, a3real pixelRadius, a3real dt);

	//Done
	a3ret FluidGridDiffuse_GPU(a3_FluidGrid_GPU* gridData, a3_UniformBuffer* currBuff, a3_UniformBuffer* prevBuff, a3i32 direction, a3real dt);

	//TODO
	a3ret FluidGridProject_GPU(a3_FluidGrid_GPU* gridData, a3real dt);

	//DONE
	a3ret FluidGridAdvect_GPU(a3_FluidGrid_GPU* gridData, a3_UniformBuffer* currBuff, a3_UniformBuffer* prevBuff, a3_UniformBuffer* v, a3_UniformBuffer* u, a3real dt, a3i32 dir);

	//DONE
	a3ret FluidGridSetBND_GPU(a3_FluidGrid_GPU* gridData, a3_UniformBuffer* sourceBuff, a3i32 direction);

	//DONE
	a3ret FluidGridSwap_GPU(a3_FluidGrid_GPU* gridData, a3_UniformBuffer* sourceBuff, a3_UniformBuffer* sourceBuff2);

	//TODO
	a3ret FluidGridVelStep_GPU(a3_FluidGrid_GPU* gridData, a3real dt);

	//DONE
	a3ret FluidDensityStep_GPU(a3_FluidGrid_GPU* gridData, a3_UniformBuffer* currBuff, a3_UniformBuffer* prevBuff, a3real dt);

	//TODO
	a3ret RunFluidSim_GPU(a3_FluidGrid_GPU* gridData, a3real dt);

	//TODO
	a3ret FreeFluidGrid_GPU(a3_FluidGrid_GPU* gridData);

#ifdef __cplusplus
}
#endif	// __cplusplus


#endif // #ifndef __ANIMAL3D_A3DX_FLUID_GPU_H