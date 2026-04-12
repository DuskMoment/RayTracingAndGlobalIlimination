#include "../a3_Fluid_GPU.h"

#include "./A3_DEMO/a3_Scene_Rendering.h"

//-----------------------------------------------------------------------------

a3ret InitFluidGrid_GPU(a3_FluidGrid_GPU* gridData, a3real diffuseK, a3ui32 diffuse_GS_Loops)
{
	a3i32 handle = -1;
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



	const a3_ShaderProgram* currShaderProgram = gridData->prog_step_init_grid;
	a3shaderProgramActivate(currShaderProgram);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, gridData->densityBuffer->handle->handle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, gridData->densityBuffer->handle->handle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	// exe
	glDispatchCompute((GRID_LENGTH + 31) / 32, (GRID_LENGTH + 31) / 32, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, gridData->prevDensityBuffer->handle->handle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, gridData->prevDensityBuffer->handle->handle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	// exe
	glDispatchCompute((GRID_LENGTH + 31) / 32, (GRID_LENGTH + 31) / 32, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, gridData->velocityBufferU->handle->handle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, gridData->velocityBufferU->handle->handle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	// exe
	glDispatchCompute((GRID_LENGTH + 31) / 32, (GRID_LENGTH + 31) / 32, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, gridData->prevVelocityBufferU->handle->handle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, gridData->prevVelocityBufferU->handle->handle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	// exe
	glDispatchCompute((GRID_LENGTH + 31) / 32, (GRID_LENGTH + 31) / 32, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, gridData->velocityBufferV->handle->handle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, gridData->velocityBufferV->handle->handle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	// exe
	glDispatchCompute((GRID_LENGTH + 31) / 32, (GRID_LENGTH + 31) / 32, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, gridData->prevVelocityBufferV->handle->handle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, gridData->prevVelocityBufferV->handle->handle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	// exe
	glDispatchCompute((GRID_LENGTH + 31) / 32, (GRID_LENGTH + 31) / 32, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);



	gridData->diffuseConstant = diffuseK;
	gridData->diffuse_GS_Loops = diffuse_GS_Loops;

	/*gridData->prog_step_difuse = NULL;
	gridData->prog_step_advect = NULL;
	gridData->prog_step_project_div = NULL;
	gridData->prog_step_project_hodgeDe = NULL;
	gridData->prog_step_project_poisson = NULL;
	gridData->prog_step_setBND = NULL;
	gridData->prog_step_add_source_from_grid = NULL;
	gridData->prog_step_add_source_from_point = NULL;*/

	/*gridData->densityBufferData = NULL;
	gridData->prevDensityBufferData = NULL;
	gridData->velocityBufferUData = NULL;
	gridData->prevVelocityBufferUData = NULL;
	gridData->velocityBufferVData = NULL;
	gridData->prevVelocityBufferVData = NULL;*/

	return 1;
}

a3ret FluidGridAddSourceFromGrid_GPU(a3_FluidGrid_GPU* gridData, a3_UniformBuffer* dest, a3_UniformBuffer* source, a3real dt)
{

	a3real output[1600];
	const a3_ShaderProgram* currShaderProgram = gridData->prog_step_add_source_from_grid;
	
	//bind density
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, source->handle->handle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, source->handle->handle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	//bind prev density
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, dest->handle->handle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, dest->handle->handle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);


	a3shaderUniformSendFloat(a3unif_single, a3shaderUniformGetLocation(currShaderProgram, "uDeltaTime"), 1, &dt);

	// exe
	glDispatchCompute((GRID_LENGTH + 31) / 32, (GRID_LENGTH + 31) / 32, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);


	glBindBuffer(GL_SHADER_STORAGE_BUFFER, dest->handle->handle);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, (1600) * sizeof(a3real), output);

	return 1;
}

a3ret FluidGridAddSourceFromPoint_GPU(a3_FluidGrid_GPU* gridData, a3_UniformBuffer* source, a3vec2 point, a3real vel, a3real pixelRadius, a3real dt)
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
	a3shaderUniformSendFloat(a3unif_single, a3shaderUniformGetLocation(currShaderProgram, "uVelocity"), 1, &vel);

	//exe
	glDispatchCompute((GRID_LENGTH + 31) / 32, (GRID_LENGTH + 31) / 32, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	//read
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, source->handle->handle);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, (1600) * sizeof(a3real), output);

	return 1;
}

a3ret FluidGridDiffuse_GPU(a3_FluidGrid_GPU* gridData, a3_UniformBuffer* currBuff, a3_UniformBuffer* prevBuff, a3i32 direction, a3real dt)
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

		//read -- debug
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, currBuff->handle->handle);
		glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, (1600) * sizeof(a3real), output);

		FluidGridSetBND_GPU(gridData, currBuff, direction);
	}

	//read -- debug
	/*glBindBuffer(GL_SHADER_STORAGE_BUFFER, currBuff->handle->handle);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, (1600) * sizeof(a3real), output);*/

	return 1;
}

//this one is going to suck :/
a3ret FluidGridProject_GPU(a3_FluidGrid_GPU* gridData, a3real dt)
{

	a3real output[1600];

	//solve divergance
	//bnd twice
	//bind
	a3_UniformBuffer* u = gridData->velocityBufferU;
	a3_UniformBuffer* v = gridData->velocityBufferV;
	a3_UniformBuffer* u0 = gridData->prevVelocityBufferU;
	a3_UniformBuffer* v0 = gridData->prevVelocityBufferV;

	const a3_ShaderProgram* currShaderProgram = gridData->prog_step_project_div;
	a3shaderProgramActivate(currShaderProgram);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, u->handle->handle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, u->handle->handle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, v->handle->handle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, v->handle->handle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, u0->handle->handle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, u0->handle->handle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, v0->handle->handle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, v0->handle->handle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	//exe
	glDispatchCompute((GRID_LENGTH + 31) / 32, (GRID_LENGTH + 31) / 32, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	//read -- debug
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, v0->handle->handle);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, (1600) * sizeof(a3real), output);


	FluidGridSetBND_GPU(gridData, v0, 0);
	FluidGridSetBND_GPU(gridData, u0, 0);
	
	//loop for a GL
	//posson
	//bnd

	currShaderProgram = gridData->prog_step_project_poisson;
	for (a3ui32 i = 0; i < gridData->diffuse_GS_Loops; i++)
	{


		glBindBuffer(GL_SHADER_STORAGE_BUFFER, u0->handle->handle);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, u0->handle->handle);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, v0->handle->handle);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, v0->handle->handle);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);


		//exe
		glDispatchCompute((GRID_LENGTH + 31) / 32, (GRID_LENGTH + 31) / 32, 1);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);

		//read -- debug
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, u0->handle->handle);
		glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, (1600) * sizeof(a3real), output);


		FluidGridSetBND_GPU(gridData, u0, 0);

	}
	
	//hodge decomposition
	//bnd twice
	currShaderProgram = gridData->prog_step_project_hodgeDe;


	glBindBuffer(GL_SHADER_STORAGE_BUFFER, u->handle->handle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, u->handle->handle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, v->handle->handle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, v->handle->handle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, u0->handle->handle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, u0->handle->handle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, v0->handle->handle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, v0->handle->handle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	//exe
	glDispatchCompute((GRID_LENGTH + 31) / 32, (GRID_LENGTH + 31) / 32, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);


	//read -- debug
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, v->handle->handle);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, (1600) * sizeof(a3real), output);


	FluidGridSetBND_GPU(gridData, v, 2);
	FluidGridSetBND_GPU(gridData, u, 1);



	return 1;
}

a3ret FluidGridAdvect_GPU(a3_FluidGrid_GPU* gridData, a3_UniformBuffer* currBuff, a3_UniformBuffer* prevBuff, a3_UniformBuffer* v, a3_UniformBuffer* u, a3real dt, a3i32 dir)
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
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, currBuff->handle->handle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, currBuff->handle->handle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);


	//prev density
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, prevBuff->handle->handle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, prevBuff->handle->handle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);


	a3shaderUniformSendFloat(a3unif_single, a3shaderUniformGetLocation(currShaderProgram, "uDt"), 1, &dt);
	a3shaderUniformSendInt(a3unif_single, a3shaderUniformGetLocation(currShaderProgram, "uDir"), 1, &dir);


	//exe
	glDispatchCompute((GRID_LENGTH + 31) / 32, (GRID_LENGTH + 31) / 32, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	//read
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, currBuff->handle->handle);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, (1600) * sizeof(a3real), output);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, prevBuff->handle->handle);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, (1600) * sizeof(a3real), output2);

	FluidGridSetBND_GPU(gridData, currBuff, dir);


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
	glDispatchCompute(1,1,1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	return 1;
}

a3ret FluidGridSwap_GPU(a3_FluidGrid_GPU* gridData, a3_UniformBuffer* sourceBuff, a3_UniformBuffer* sourceBuff2)
{
	a3real output[1600];
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

	//read
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, sourceBuff->handle->handle);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, (1600) * sizeof(a3real), output);

	return 1;
}

a3ret FluidGridVelStep_GPU(a3_FluidGrid_GPU* gridData, a3real dt)
{
	
	//exe
	//glDispatchCompute((GRID_LENGTH + 31) / 32, (GRID_LENGTH + 31) / 32, 1);
	//glMemoryBarrier(GL_ALL_BARRIER_BITS);

	/*
	FluidGridAddSource(gridSize, u, u0, dt);
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
	FluidGridProject(N, u, v, u0, v0);
	*/

	FluidGridAddSourceFromGrid_GPU(gridData, gridData->velocityBufferV, gridData->prevVelocityBufferV, dt);
	FluidGridAddSourceFromGrid_GPU(gridData, gridData->velocityBufferU, gridData->prevVelocityBufferU, dt);

	FluidGridSwap_GPU(gridData, gridData->velocityBufferU, gridData->prevVelocityBufferU);

	FluidGridDiffuse_GPU(gridData, gridData->velocityBufferU, gridData->prevVelocityBufferU, 1, dt);

	FluidGridSwap_GPU(gridData, gridData->velocityBufferV, gridData->prevVelocityBufferV);

	FluidGridDiffuse_GPU(gridData, gridData->velocityBufferV, gridData->prevVelocityBufferV, 1, dt);

	FluidGridProject_GPU(gridData, dt); 

	FluidGridSwap_GPU(gridData, gridData->velocityBufferU, gridData->prevVelocityBufferU);
	FluidGridSwap_GPU(gridData, gridData->velocityBufferV, gridData->prevVelocityBufferV);


	FluidGridAdvect_GPU(gridData, gridData->velocityBufferU, gridData->prevVelocityBufferU, gridData->prevVelocityBufferU, gridData->prevVelocityBufferV, dt, 1);
	FluidGridAdvect_GPU(gridData, gridData->velocityBufferV, gridData->prevVelocityBufferV, gridData->prevVelocityBufferU, gridData->prevVelocityBufferV, dt, 2);


	FluidGridProject_GPU(gridData, dt);
	return 1;
}

a3ret FluidDensityStep_GPU(a3_FluidGrid_GPU* gridData, a3_UniformBuffer* currBuff, a3_UniformBuffer* prevBuff, a3real dt)
{
	FluidGridAddSourceFromGrid_GPU(gridData, currBuff, prevBuff, dt);
	//Swap(x0, x, (N + 2) * (N + 2));
	FluidGridSwap_GPU(gridData, currBuff, prevBuff);
	FluidGridDiffuse_GPU(gridData, currBuff, prevBuff, 0, dt);
	FluidGridSwap_GPU(gridData, currBuff, prevBuff);
	//Swap(x0, x, (N + 2) * (N + 2));
	FluidGridAdvect_GPU(gridData, currBuff, prevBuff, 
		gridData->velocityBufferU, gridData->velocityBufferV, dt, 0);

	return 1;
}

a3ret RunFluidSim_GPU(a3_FluidGrid_GPU* gridData, a3real dt)
{
	a3vec2 point;
	point.x = 400;
	point.y = 400;

	//FluidGridAddSourceFromPoint_GPU(gridData, gridData->densityBuffer, point, 1000, 1, dt);
	//FluidGridAddSourceFromPoint_GPU(gridData, gridData->velocityBufferV, point, 10000, dt);
	//FluidGridAddSourceFromPoint_GPU(gridData, gridData->velocityBufferU, point, 10000, dt);

	FluidGridVelStep_GPU(gridData, dt);
	FluidDensityStep_GPU(gridData, gridData->prevDensityBuffer, gridData->densityBuffer, dt);

	////FluidGridVelStep_GPU(gridData, shaderPrograms, dt);
	////FluidDensityStep_GPU(gridData, shaderPrograms, dt);

	return 1;
}

a3ret FreeFluidGrid_GPU(a3_FluidGrid_GPU* gridData)
{
	if (gridData == NULL) { return 0; }

	/*if (gridData->densityBufferData != NULL) { free(gridData->densityBufferData); }
	if (gridData->prevDensityBufferData != NULL) { free(gridData->prevDensityBufferData); }
	if (gridData->velocityBufferU != NULL) { free(gridData->velocityBufferU); }
	if (gridData->prevVelocityBufferU != NULL) { free(gridData->prevVelocityBufferU); }
	if (gridData->velocityBufferV != NULL) { free(gridData->velocityBufferV); }
	if (gridData->prevVelocityBufferV != NULL) { free(gridData->prevVelocityBufferV); }*/

	free(gridData);;

	return 1;
}
