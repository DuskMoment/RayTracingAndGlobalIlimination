#include "../a3_Fluid_GPU.h"

#include "./A3_DEMO/a3_Scene_Rendering.h"

//-----------------------------------------------------------------------------

a3ret InitFluidGrid_GPU(a3_FluidGrid_GPU* gridData, a3ui32 diffuse_GS_Loops)
{
	a3i32 handle;
	glGenBuffers(1, &handle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, handle);
	glBufferData(GL_SHADER_STORAGE_BUFFER, (GRID_SIZE) * sizeof(a3real), NULL, GL_STREAM_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, handle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	gridData->densityBuffer->handle->handle = handle;

	//prev density
	glGenBuffers(1, &handle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, handle);
	glBufferData(GL_SHADER_STORAGE_BUFFER, (GRID_SIZE) * sizeof(a3real), NULL, GL_STREAM_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, handle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	gridData->prevDensityBuffer->handle->handle = handle;

	glGenBuffers(1, &handle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, handle);
	glBufferData(GL_SHADER_STORAGE_BUFFER, (GRID_SIZE) * sizeof(a3real), NULL, GL_STREAM_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, handle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	gridData->velocityBufferU->handle->handle = handle;

	glGenBuffers(1, &handle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, handle);
	glBufferData(GL_SHADER_STORAGE_BUFFER, (GRID_SIZE) * sizeof(a3real), NULL, GL_STREAM_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, handle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	gridData->prevVelocityBufferU->handle->handle = handle;


	glGenBuffers(1, &handle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, handle);
	glBufferData(GL_SHADER_STORAGE_BUFFER, (GRID_SIZE) * sizeof(a3real), NULL, GL_STREAM_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, handle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	gridData->prevVelocityBufferV->handle->handle = handle;


	glGenBuffers(1, &handle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, handle);
	glBufferData(GL_SHADER_STORAGE_BUFFER, (GRID_SIZE) * sizeof(a3real), NULL, GL_STREAM_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, handle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	gridData->velocityBufferV->handle->handle = handle;

	gridData->diffuse_GS_Loops = diffuse_GS_Loops;

	gridData->prog_step_difuse = NULL;
	gridData->prog_step_advect = NULL;
	gridData->prog_step_project_div = NULL;
	gridData->prog_step_project_hodgeDe = NULL;
	gridData->prog_step_project_poisson = NULL;
	gridData->prog_step_setBND = NULL;
	gridData->prog_step_add_source_from_grid = NULL;
	gridData->prog_step_add_source_from_point = NULL;


	return 1;
}

a3ret FluidGridAddSourceFromGrid_GPU(a3_FluidGrid_GPU* gridData, a3_UniformBuffer* source, a3_UniformBuffer* dest, a3real dt)
{
	//bind density
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, source->handle->handle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, source->handle->handle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	//bind prev density
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, dest->handle->handle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, dest->handle->handle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	return 1;
}

a3ret FluidGridAddSourceFromPoint_GPU(a3_FluidGrid_GPU* gridData, a3_UniformBuffer* source, a3vec2 point, a3real pixelRadius, a3real dt)
{
	a3real output[1600];
	const a3_ShaderProgram* currShaderProgram = gridData->prog_step_add_source_from_point;
	a3shaderProgramActivate(currShaderProgram);

	//bind
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, source->handle->handle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, source->handle->handle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	a3shaderUniformSendFloat(a3unif_single, a3shaderUniformGetLocation(currShaderProgram, "uDeltaTime"), 1, &dt);
	a3shaderUniformSendFloat(a3unif_single, a3shaderUniformGetLocation(currShaderProgram, "uPixelRadius"), 1, &pixelRadius);
	a3shaderUniformSendFloat(a3unif_vec2, a3shaderUniformGetLocation(currShaderProgram, "uPoint"), 1, point.v);

	//exe
	glDispatchCompute((GRID_LENGTH + 31) / 32, (GRID_LENGTH + 31) / 32, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	//read
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, source->handle->handle);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, (1600) * sizeof(a3real), output);

	return 1;
}

a3ret FluidGridDiffuse_GPU(a3_FluidGrid_GPU* gridData, a3_UniformBuffer* prevBuff, a3_UniformBuffer* currBuff, a3i32 direction, a3real dt)
{
	a3real output[1600];
	for (a3ui32 i = 0; i < gridData->diffuse_GS_Loops; i++)
	{
		const a3_ShaderProgram* currShaderProgram = gridData->prog_step_difuse;
		a3shaderProgramActivate(currShaderProgram);

		//bind
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, prevBuff->handle->handle);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, prevBuff->handle->handle);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, currBuff->handle->handle);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, currBuff->handle->handle);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		a3shaderUniformSendFloat(a3unif_single, a3shaderUniformGetLocation(currShaderProgram, "uDiffuseConstant"), 1, &gridData->diffuseConstant);
		a3shaderUniformSendFloat(a3unif_single, a3shaderUniformGetLocation(currShaderProgram, "uDeltaTime"), 1, &dt);
		
		//exe
		glDispatchCompute((GRID_LENGTH + 31) / 32, (GRID_LENGTH + 31) / 32, 1);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);

		//read
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, currBuff->handle->handle);
		glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, (1600) * sizeof(a3real), output);

		FluidGridSetBND_GPU(gridData, currBuff, direction);
	}

	//read
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, currBuff->handle->handle);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, (1600) * sizeof(a3real), output);

	return 1;
}

a3ret FluidGridAdvect_GPU(a3_FluidGrid_GPU* gridData, a3_UniformBuffer* d, a3_UniformBuffer* d0, a3_UniformBuffer* v, a3_UniformBuffer* u, a3real dt, a3i32 dir)
{

	a3real output[1600];
	a3real output2[1600];


	const a3_ShaderProgram* currShaderProgram = gridData->prog_step_advect;
	a3shaderProgramActivate(currShaderProgram);

	//velocity u
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, u->handle->handle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, u->handle->handle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);


	//velocity v
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, v->handle->handle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, v->handle->handle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	//bind
//density
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, d->handle->handle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, d->handle->handle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);


	//prev density
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, d0->handle->handle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, d0->handle->handle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);


	a3shaderUniformSendFloat(a3unif_single, a3shaderUniformGetLocation(currShaderProgram, "uDt"), 1, &dt);
	a3shaderUniformSendInt(a3unif_single, a3shaderUniformGetLocation(currShaderProgram, "uDir"), 1, &dir);


	//exe
	glDispatchCompute((GRID_LENGTH + 31) / 32, (GRID_LENGTH + 31) / 32, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	//read
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, d->handle->handle);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, (1600) * sizeof(a3real), output);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, d0->handle->handle);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, (1600) * sizeof(a3real), output2);

	FluidGridSetBND_GPU(gridData, d, dir);


	return 1;
}

a3ret FluidGridSetBND_GPU(a3_FluidGrid_GPU* gridData, a3_UniformBuffer* sourceBuff, a3i32 direction)
{
	const a3_ShaderProgram* currShaderProgram = gridData->prog_step_setBND;
	a3shaderProgramActivate(currShaderProgram);

	//bind
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, sourceBuff->handle->handle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, sourceBuff->handle->handle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	a3shaderUniformSendInt(a3unif_single, a3shaderUniformGetLocation(currShaderProgram, "uDirection"), 1, &direction);

	//exe
	glDispatchCompute((GRID_LENGTH + 31) / 32, (GRID_LENGTH + 31) / 32, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	return 1;
}

a3ret FluidGridSwap_GPU(a3_FluidGrid_GPU* gridData, a3_UniformBuffer* sourceBuff, a3_UniformBuffer* sourceBuff2)
{
	const a3_ShaderProgram* currShaderProgram = gridData->prog_step_swap_buffers;
	a3shaderProgramActivate(currShaderProgram);

	//bind
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, sourceBuff->handle->handle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, sourceBuff->handle->handle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);


	glBindBuffer(GL_SHADER_STORAGE_BUFFER, sourceBuff2->handle->handle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, sourceBuff2->handle->handle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glDispatchCompute((GRID_LENGTH + 31) / 32, (GRID_LENGTH + 31) / 32, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);



	return 1;
}

a3ret FluidGridVelStep_GPU(a3_FluidGrid_GPU* gridData, a3real dt)
{
	//bind density
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, gridData->densityBuffer->handle->handle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, gridData->densityBuffer->handle->handle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	//bind prev density
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, gridData->densityBuffer->handle->handle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, gridData->densityBuffer->handle->handle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);


	//bind velocity
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, gridData->densityBuffer->handle->handle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, gridData->densityBuffer->handle->handle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	//bind prev velocity
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, gridData->densityBuffer->handle->handle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, gridData->densityBuffer->handle->handle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	//exe
	//glDispatchCompute((GRID_LENGTH + 31) / 32, (GRID_LENGTH + 31) / 32, 1);
	//glMemoryBarrier(GL_ALL_BARRIER_BITS);

	/*FluidGridAddSource(gridSize, u, u0, dt);
	FluidGridAddSource(N, v, v0, dt);
	Swap(u0, u, (N + 2) * (N + 2));
	FluidGridDiffuse(N, 1, u, u0, visc, dt);
	Swap(v0, v, (N + 2) * (N + 2));
	FluidGridDiffuse(N, 2, v, v0, visc, dt);
	FluidGridProject(N, u, v, u0, v0);
	Swap(u0, u, (N + 2) * (N + 2));
	Swap(v0, v, (N + 2) * (N + 2));
	FluidGridAdvect(N, 1, u, u0, u0, v0, dt);
	FluidGridAdvect(N, 2, v, v0, u0, v0, dt);
	FluidGridProject(N, u, v, u0, v0);*/

	return 1;
}

a3ret FluidDensityStep_GPU(a3_FluidGrid_GPU* gridData, a3_UniformBuffer* prevBuff, a3_UniformBuffer* currBuff, a3real dt)
{
	FluidGridAddSourceFromGrid_GPU(gridData, prevBuff, currBuff, dt);
	//Swap(x0, x, (N + 2) * (N + 2));
	FluidGridSwap_GPU(gridData, prevBuff, currBuff);
	FluidGridDiffuse_GPU(gridData, prevBuff, currBuff, 0, dt);
	FluidGridSwap_GPU(gridData, prevBuff, currBuff);
	//Swap(x0, x, (N + 2) * (N + 2));
	FluidGridAdvect_GPU(gridData, currBuff, prevBuff, 
		gridData->velocityBufferU, gridData->velocityBufferV, dt, 0);

	return 1;
}

a3ret RunFluidSim_GPU(a3_FluidGrid_GPU* gridData, a3real dt)
{
	FluidGridAddSourceFromPoint_GPU(gridData, gridData->densityBuffer, a3vec2_zero, 10, dt);
	FluidDensityStep_GPU(gridData, gridData->prevDensityBuffer, gridData->densityBuffer, dt);

	////FluidGridVelStep_GPU(gridData, shaderPrograms, dt);
	////FluidDensityStep_GPU(gridData, shaderPrograms, dt);

	return 1;
}

a3ret FreeFluidGrid_GPU(a3_FluidGrid_GPU* gridData)
{
	if (gridData == NULL) { return 0; }

	if (gridData->densityBufferData != NULL) { free(gridData->densityBufferData); }
	if (gridData->prevDensityBufferData != NULL) { free(gridData->prevDensityBufferData); }
	if (gridData->velocityBufferU != NULL) { free(gridData->velocityBufferU); }
	if (gridData->prevVelocityBufferU != NULL) { free(gridData->prevVelocityBufferU); }
	if (gridData->velocityBufferV != NULL) { free(gridData->velocityBufferV); }
	if (gridData->prevVelocityBufferV != NULL) { free(gridData->prevVelocityBufferV); }

	free(gridData);;

	return 1;
}
