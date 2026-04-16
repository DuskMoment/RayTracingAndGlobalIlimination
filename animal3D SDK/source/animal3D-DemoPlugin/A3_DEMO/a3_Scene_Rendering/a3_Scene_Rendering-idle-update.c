/*
	Copyright 2011-2026 Daniel S. Buckstein

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

		http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

/*
	animal3D SDK: Minimal 3D Animation Framework
	By Daniel S. Buckstein
	
	a3_Scene_Rendering-idle-update.c/.cpp
	Demo mode implementations: rendering scene.

	********************************************
	*** UPDATE FOR RENDERING SCENE MODE      ***
	********************************************
*/

//-----------------------------------------------------------------------------

#include "../a3_Scene_Rendering.h"

//typedef struct a3_DemoState a3_DemoState;
#include "../a3_DemoState.h"

#include "../_a3_scene_utilities/a3_SceneMacros.h"
#include "../_physics/a3_Fluid.h"

#include <time.h>


//-----------------------------------------------------------------------------
// UTILS

void a3demo_update_objects(a3f64 const dt, a3_SceneObject* sceneObjectBase,
	a3ui32 count, a3boolean useZYX, a3boolean applyScale);
void a3demo_update_defaultAnimation(a3_DemoState* demoState, a3f64 const dt,
	a3_SceneObject* sceneObjectBase, a3ui32 count, a3ui32 axis);
void a3demo_update_bindSkybox(a3_SceneObject* obj_camera, a3_SceneObject* obj_skybox);
void a3demo_update_pointLight(a3_SceneObject* obj_camera, a3_ScenePointLight* pointLightBase, a3ui32 count);

void a3demo_applyScale_internal(a3_SceneObject* sceneObject, a3real4x4p s);


//-----------------------------------------------------------------------------
// UPDATE

void a3rendering_update_sceneGraph(a3_Scene_Rendering* scene, a3f64 const dt)
{
	a3ui32 i;
	a3mat4 scaleMat = a3mat4_identity;

	a3demo_update_objects(dt, scene->object_scene, renderingMaxCount_sceneObject, 0, 0);
	a3demo_update_objects(dt, scene->obj_camera_main, 1, 1, 0);

	a3scene_updateProjectorViewProjectionMat(scene->proj_camera_main);

	// apply scales to objects
	for (i = 0; i < renderingMaxCount_sceneObject; ++i)
	{
		a3demo_applyScale_internal(scene->object_scene + i, scaleMat.m);
	}

	// update skybox
	a3demo_update_bindSkybox(scene->obj_camera_main, scene->obj_skybox);

	// the scene graph is also a hierarchy - update FK
	for (i = 0; i < renderingMaxCount_sceneObject; ++i)
		scene->sceneGraphState->localSpace->hpose_base[i].transformMat = scene->object_scene[i].modelMat;
	
	a3kinematicsSolveForward(scene->sceneGraphState);
	a3hierarchyStateUpdateLocalInverse(scene->sceneGraphState);
	a3hierarchyStateUpdateObjectInverse(scene->sceneGraphState);
}

void a3rendering_update_other(
    a3_Scene_Rendering* scene, a3f64 const dt)
{
    a3real const rps = a3real_threesixty * 0.05f;
    a3real angle;

    scene->rotate_time += (a3real)dt;

    angle = a3trigValid_sind((a3real)scene->rotate_time * rps);

    scene->obj_material_ball[0].euler.z = angle;
    scene->obj_material_ball[1].euler.z = angle;
    scene->obj_material_ball[2].euler.z = angle;
}

void a3rendering_update(a3_DemoState* demoState, a3_Scene_Rendering* scene, a3f64 const dt)
{
	a3ui32 i;

	// active camera
	a3_SceneProjector const* activeCamera = scene->projector + scene->activeCamera;
	a3_SceneObject const* activeCameraObject = activeCamera->sceneObject;

    if (demoState->updateAnimation)
    {
        a3rendering_update_other(scene, dt);
    }

    // bias matrix
    const a3mat4 bias = {
        0.5f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.5f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.5f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f,
    };
    const a3mat4 unbias = {
         2.0f,  0.0f,  0.0f, 0.0f,
         0.0f,  2.0f,  0.0f, 0.0f,
         0.0f,  0.0f,  2.0f, 0.0f,
        -1.0f, -1.0f, -1.0f, 1.0f,
    };

	// update scene graph local transforms
	a3rendering_update_sceneGraph(scene, dt);

	// update model matrix stack data using scene graph
	for (i = 0; i < renderingMaxCount_sceneObject; ++i)
	{
		a3scene_updateModelMatrixStack(scene->modelMatrixStack + i,
			activeCamera->projectionMat.m,
			scene->sceneGraphState->objectSpace->hpose_base[activeCameraObject->sceneGraphIndex].transformMat.m,
			scene->sceneGraphState->objectSpaceInv->hpose_base[activeCameraObject->sceneGraphIndex].transformMat.m,
			scene->sceneGraphState->objectSpace->hpose_base[scene->object_scene[i].sceneGraphIndex].transformMat.m,
			a3mat4_identity.m);
	}

    // update viewer matrix stack data using scene graph
    for (i = 0; i < renderingMaxCount_projector; ++i)
    {
        activeCamera = scene->projector + i;
        activeCameraObject = activeCamera->sceneObject;
        a3scene_updateViewerMatrixStack(scene->viewerMatrixStack + i,
            scene->sceneGraphState->objectSpace->hpose_base[activeCameraObject->sceneGraphIndex].transformMat.m,
            scene->sceneGraphState->objectSpaceInv->hpose_base[activeCameraObject->sceneGraphIndex].transformMat.m,
            activeCamera->projectionMat.m,
            activeCamera->projectionMatInv.m,
            bias.m, unbias.m);
    }

	/*a3real dtest[GRID_SIZE];
	a3real fade[GRID_SIZE];
	for (int i = 0; i < GRID_SIZE; i++)
	{
		dtest[i] = 0;
	}

	for (int i = 800; i < 805; i++) {
		dtest[i] = 1;
	}

	for (int i = 0; i < GRID_SIZE; i++)
	{
		fade[i] = scene->fluidGrid->fade;
	}*/

	/*FluidGridAddSource(scene->fluidGrid->size, scene->fluidGrid->density, dtest, (a3real)1);
	FluidGridFade(scene->fluidGrid->size, scene->fluidGrid->density, fade, (a3real)1);

	srand((unsigned int)time(NULL));

	a3real test[GRID_SIZE];

	for (int i = 0; i < GRID_SIZE; i++)
	{
		test[i] = (((float)rand() / (float)RAND_MAX) * (a3real)200.0) - (a3real)100;
	}

	srand(5);

	a3real utest[GRID_SIZE];

	for (int i = 0; i < GRID_SIZE; i++)
	{
		utest[i] = (((float)rand() / (float)RAND_MAX) * (a3real)200.0) - (a3real)100;
	}

	for (int i = 0; i < GRID_SIZE; i++)
	{
		dtest[i] = ((float)rand() / (float)RAND_MAX);
	}

	FluidGridAddSource(scene->fluidGrid->size, scene->fluidGrid->velocityU, utest, (a3real)0.016);
	FluidGridAddSource(scene->fluidGrid->size, scene->fluidGrid->velocityV, test, (a3real)0.016);


	FluidGridSim(scene->fluidGrid, scene->fluidGrid->length, scene->fluidGrid->velocityU, scene->fluidGrid->velocityV, (a3real) 0.0, (a3real)0.001);*/
	
	a3vec2 point;
	point.x = (a3real)demoState->mouse->x;
	point.y = GRID_LENGTH - (a3real)demoState->mouse->y;

	a3vec2 mouseVel;
	mouseVel.x = (a3real)demoState->mouse->x - (a3real)demoState->mouse->x0;
	mouseVel.y = (GRID_LENGTH - (a3real)demoState->mouse->y) - (GRID_LENGTH - (a3real)demoState->mouse->y0);

	FluidGridAddSourceFromPoint_GPU(scene->fluidGrid_GPU, scene->fluidGrid_GPU->densityBuffer, point, (a3real)0.2, 1000, (a3real)0.1);

	FluidGridAddSourceFromPoint_GPU(scene->fluidGrid_GPU, scene->fluidGrid_GPU->velocityBufferU, point, mouseVel.x * (a3real)10000, 1000, (a3real)1);
	FluidGridAddSourceFromPoint_GPU(scene->fluidGrid_GPU, scene->fluidGrid_GPU->velocityBufferV, point, mouseVel.y * (a3real)10000, 1000, (a3real)1);
}


//-----------------------------------------------------------------------------
