#include "../a3_Fluid_GPU.h"
#include <math.h>

#include <stdio.h>
#include <gl/glew.h>

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

a3ret FluidGridVelStep_GPU(a3_FluidGrid_GPU* gridData, a3ui32 gridSize)
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

	return 1;
}

a3ret FluidDensityStep_GPU(a3_FluidGrid_GPU* gridData, a3ui32 gridSize)
{
	return 1;
}

a3ret RunFluidSim_GPU(a3_FluidGrid_GPU* gridData, a3i32 gridSize, a3real dt) 
{
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
