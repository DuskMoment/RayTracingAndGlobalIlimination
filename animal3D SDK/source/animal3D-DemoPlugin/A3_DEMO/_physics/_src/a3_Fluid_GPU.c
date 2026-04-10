#include "../a3_Fluid_GPU.h"

#include "./A3_DEMO/a3_Scene_Rendering.h"

//-----------------------------------------------------------------------------

a3ret InitFluidGrid_GPU(a3_FluidGrid_GPU* gridData, a3ui32 gridSize)
{
	//generate the buffer
	a3i32 handle;

	//density
	glGenBuffers(1, &handle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, handle);
	glBufferData(GL_SHADER_STORAGE_BUFFER, (gridSize) * sizeof(a3real), gridData->densityBufferData, GL_STREAM_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, handle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	gridData->densityBuffer->handle->handle = handle;

	//prev density
	glGenBuffers(1, &handle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, handle);
	glBufferData(GL_SHADER_STORAGE_BUFFER, (gridSize) * sizeof(a3real), gridData->prevDensityBuffer, GL_STREAM_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, handle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	gridData->prevDensityBuffer->handle->handle = handle;

	glGenBuffers(1, &handle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, handle);
	glBufferData(GL_SHADER_STORAGE_BUFFER, (gridSize) * sizeof(a3real), gridData->velocityBufferU, GL_STREAM_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, handle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	gridData->velocityBufferU->handle->handle = handle;

	glGenBuffers(1, &handle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, handle);
	glBufferData(GL_SHADER_STORAGE_BUFFER, (gridSize) * sizeof(a3real), gridData->prevVelocityBufferU, GL_STREAM_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, handle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	gridData->prevVelocityBufferU->handle->handle = handle;


	glGenBuffers(1, &handle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, handle);
	glBufferData(GL_SHADER_STORAGE_BUFFER, (gridSize) * sizeof(a3real), gridData->velocityBufferV, GL_STREAM_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, handle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	gridData->prevVelocityBufferV->handle->handle = handle;


	glGenBuffers(1, &handle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, handle);
	glBufferData(GL_SHADER_STORAGE_BUFFER, (gridSize) * sizeof(a3real), gridData->prevVelocityBufferV, GL_STREAM_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, handle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	gridData->prevVelocityBufferV->handle->handle = handle;

	return 1;
}

a3ret FluidGridAddSourceFromGrid_GPU(a3_FluidGrid_GPU* gridData, a3_ShaderProgram* shaderProgram, a3_UniformBuffer* source, a3_UniformBuffer* dest)
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

a3ret FluidGridAddSourceFromPoint_GPU(a3_FluidGrid_GPU* gridData, a3_ShaderProgram* shaderProgram, a3_UniformBuffer* source, a3vec2 point, a3ui32 pixelRadius)
{
	//bind density
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, source->handle->handle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, source->handle->handle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	a3shaderProgramActivate(shaderProgram);

	glDispatchCompute((GRID_LENGTH + 31) / 32, (GRID_LENGTH + 31) / 32, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	return 1;
}

a3ret FluidGridVelStep_GPU(a3_FluidGrid_GPU* gridData, a3_ShaderProgram* shaderProgram, a3real dt)
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

a3ret FluidDensityStep_GPU(a3_FluidGrid_GPU* gridData, a3_ShaderProgram* shaderProgram, a3real dt)
{
	return 1;
}

a3ret RunFluidSim_GPU(a3_FluidGrid_GPU* gridData, a3_ShaderProgram* shaderPrograms, a3real dt)
{
	FluidGridVelStep_GPU(gridData, shaderPrograms, dt);
	FluidDensityStep_GPU(gridData, shaderPrograms, dt);

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
