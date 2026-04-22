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

	a3_Scene_Rendering-idle-render.c/.cpp
	Demo mode implementations: rendering scene.

	********************************************
	*** RENDERING FOR RENDERING SCENE MODE   ***
	********************************************
*/

//-----------------------------------------------------------------------------

#include "../a3_Scene_Rendering.h"

#include "../a3_DemoState.h"

#include "../_a3_scene_utilities/a3_SceneRenderUtils.h"


// OpenGL
#ifdef _WIN32
#include <gl/glew.h>
#include <Windows.h>
#include <GL/GL.h>
#else	// !_WIN32
#include <OpenGL/gl3.h>
#endif	// _WIN32


//-----------------------------------------------------------------------------
																	//velocity						//doublebuffer
static void bounds(a3_DemoState const* demoState, int targetIndex, a3_Framebuffer const* fsqBuffer, a3_Framebuffer const* drawToBuffer, const a3mat4 fsq)
{
	// WRITE target
	a3framebufferActivate(drawToBuffer);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);
	glClear(GL_COLOR_BUFFER_BIT);

	// READ from previous
	a3framebufferBindColorTexture(fsqBuffer, a3tex_unit00, 0);

	// TEST texture
	//a3textureActivate(demoState->tex_earth_dm, a3tex_unit00);

	// draw fsq with active texture
	const a3_SceneShaderProgram* currentDemoProgram = demoState->prog_drawTexture;
	a3shaderProgramActivate(currentDemoProgram->program);

	const a3_VertexDrawable* currentDrawable = demoState->draw_unit_plane_z;
	a3vertexDrawableActivate(currentDrawable);

	a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, fsq.mm);
	a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uAtlas, 1, a3mat4_identity.mm);
	a3shaderUniformSendFloat(a3unif_vec4, currentDemoProgram->uColor, 1, a3vec4_one.v);

	a3vertexDrawableRenderActive();

	// draw border
	currentDemoProgram = demoState->prog_drawBorder;
	a3shaderProgramActivate(currentDemoProgram->program);
	a3f32 n = (a3f32)-1.0;
	a3shaderUniformSendFloat(a3unif_single, a3shaderUniformGetLocation(currentDemoProgram->program, "uNegate"), 1, &n);

	a3vec4 axis;
	axis.x = (a3real)fsqBuffer->frameWidth;
	axis.y = (a3real)fsqBuffer->frameHeight;
	axis.z = 1.0F / axis.x;
	axis.w = 1.0F / axis.y;
	a3shaderUniformSendFloat(a3unif_vec4, currentDemoProgram->uAxis, 1, axis.v);

	a3vertexDrawableDeactivate();
	glDrawArrays(GL_POINTS, 0, 1);

	// COMPOSITE for display only
	/*a3framebufferDeactivateSetViewport(a3fbo_depthDisable,
		-demoState->frameBorder, -demoState->frameBorder, demoState->frameWidth, demoState->frameHeight);
	currentDrawable = demoState->draw_unit_plane_z;
	currentDemoProgram = demoState->prog_drawTexture;
	a3vertexDrawableActivate(currentDrawable);
	a3shaderProgramActivate(currentDemoProgram->program);
	a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, fsq.mm);
	a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uAtlas, 1, a3mat4_identity.mm);
	a3shaderUniformSendFloat(a3unif_vec4, currentDemoProgram->uColor, 1, a3vec4_one.v);
	a3framebufferBindColorTexture(drawToBuffer, a3tex_unit00, 0);
	a3vertexDrawableRenderActive();*/
}

// controls for pipelines mode
void a3rendering_render_controls(a3_DemoState const* demoState, a3_Scene_Rendering const* scene,
	a3_TextRenderer const* text, a3vec4 const col,
	a3f32 const textAlign, a3f32 const textDepth, a3f32 const textOffsetDelta, a3f32 textOffset)
{
	//// display mode info
	//a3byte const* pipelineText[rendering_pipeline_max] = {
	//	"Forward rendering",
	//};

	//// forward pipeline names
	//a3byte const* renderProgramName[rendering_render_max] = {
	//	"Solid color",
	//	"Texture",
	//	"Lambert shading",
	//	"Phong shading",
	//	"Ray-tracing",
	//};

	//// forward display names
	//a3byte const* displayProgramName[rendering_display_max] = {
	//	"Texture",
	//};

	//// active camera name
	//a3byte const* cameraText[rendering_camera_max] = {
	//	"rendering scene camera",
	//};

	//// constant color target names
	//a3byte const colorBufferText[] = "Color target 0: FINAL DISPLAY COLOR";
	//// constant depth target name
	//a3byte const depthBufferText[] = "Depth buffer";

	//// pass names
	//a3byte const* passName[rendering_pass_max] = {
	//	"Pass: Render scene objects",
	//	"Pass: Composite",
	//};
	//a3byte const* targetText_scene[rendering_target_scene_max] = {
	//	colorBufferText,
	//	depthBufferText,
	//};
	//a3byte const* targetText_composite[rendering_target_scene_max] = {
	//	colorBufferText,
	//};
	//a3byte const* const* targetText[rendering_pass_max] = {
	//	targetText_scene,
	//	targetText_composite,
	//};

	//// pipeline and target
	//a3_Scene_Rendering_RenderProgramName const render = scene->render;
	//a3_Scene_Rendering_DisplayProgramName const display = scene->display;
	//a3_Scene_Rendering_ActiveCameraName const activeCamera = scene->activeCamera;
	//a3_Scene_Rendering_PipelineName const pipeline = scene->pipeline;
	//a3_Scene_Rendering_PassName const pass = scene->pass;
	//a3_Scene_Rendering_TargetName const targetIndex = scene->targetIndex[pass];
	//a3_Scene_Rendering_TargetName const targetCount = scene->targetCount[pass];

	//// demo modes
	//a3textDraw(text, textAlign, textOffset += textOffsetDelta, textDepth, col.r, col.g, col.b, col.a,
	//	"    Pipeline (%u / %u) ('[' | ']'): %s", pipeline + 1, rendering_pipeline_max, pipelineText[pipeline]);
	//a3textDraw(text, textAlign, textOffset += textOffsetDelta, textDepth, col.r, col.g, col.b, col.a,
	//	"    Display pass (%u / %u) ('(' | ')'): %s", pass + 1, rendering_pass_max, passName[pass]);
	//a3textDraw(text, textAlign, textOffset += textOffsetDelta, textDepth, col.r, col.g, col.b, col.a,
	//	"        Target (%u / %u) ('{' | '}'): %s", targetIndex + 1, targetCount, targetText[pass][targetIndex]);

 //   // lighting modes
 //   a3textDraw(text, textAlign, textOffset += textOffsetDelta, textDepth, col.r, col.g, col.b, col.a,
 //       "    Rendering mode (%u / %u) ('j' | 'k'): %s", render + 1, rendering_render_max, renderProgramName[render]);
 //   a3textDraw(text, textAlign, textOffset += textOffsetDelta, textDepth, col.r, col.g, col.b, col.a,
 //       "    Display mode (%u / %u) ('J' | 'K'): %s", display + 1, rendering_display_max, displayProgramName[display]);
 //   a3textDraw(text, textAlign, textOffset += textOffsetDelta, textDepth, col.r, col.g, col.b, col.a,
 //       "    Active camera (%u / %u) ('c' prev | next 'v'): %s", activeCamera + 1, rendering_camera_max, cameraText[activeCamera]);

 //   // tests
 //   a3textDraw(text, textAlign, textOffset += textOffsetDelta, textDepth, col.r, col.g, col.b, col.a,
 //       "    Test ray fired: %s", (scene->test_ray_fired ? "TRUE " : "FALSE"));
 //   a3textDraw(text, textAlign, textOffset += textOffsetDelta, textDepth, col.r, col.g, col.b, col.a,
 //       "        Hit: %s; t=%lf", (scene->test_ray_hit ? "TRUE " : "FALSE"), (a3f64)scene->test_ray_param);
}


//-----------------------------------------------------------------------------

void a3demo_uploadTransformStacks(
	a3_UniformBuffer const* ubo_transform_stacks,
	a3_SceneModelMatrixStack const* model_matrix_stacks, a3_SceneViewerMatrixStack const* viewer_matrix_stacks,
	a3ui32 const max_models, a3ui32 const num_models, a3ui32 const max_viewers, a3ui32 const num_viewers);

// sub-routine for rendering the demo state using the shading pipeline
void a3rendering_render(a3_DemoState* demoState, a3_Scene_Rendering const* scene, a3f64 const dt)
{
	// pointers
	const a3_VertexDrawable* currentDrawable;
	const a3_SceneShaderProgram* currentDemoProgram;

	// framebuffers
	const a3_Framebuffer* currentWriteFBO;
	//const a3_Framebuffer* tmpFBO;
	const a3_Framebuffer* currentReadFBO, * currentDisplayFBO;

	// indices
	//a3ui32 i, j;

	// RGB
	const a3vec4 rgba4[] = {
		{ 1.00f, 0.00f, 0.00f, 1.00f },	// red
		{ 1.00f, 0.25f, 0.00f, 1.00f },
		{ 1.00f, 0.50f, 0.00f, 1.00f },	// orange
		{ 1.00f, 0.75f, 0.00f, 1.00f },
		{ 1.00f, 1.00f, 0.00f, 1.00f },	// yellow
		{ 0.75f, 1.00f, 0.00f, 1.00f },
		{ 0.50f, 1.00f, 0.00f, 1.00f },	// lime
		{ 0.25f, 1.00f, 0.00f, 1.00f },
		{ 0.00f, 1.00f, 0.00f, 1.00f },	// green
		{ 0.00f, 1.00f, 0.25f, 1.00f },
		{ 0.00f, 1.00f, 0.50f, 1.00f },	// aqua
		{ 0.00f, 1.00f, 0.75f, 1.00f },
		{ 0.00f, 1.00f, 1.00f, 1.00f },	// cyan
		{ 0.00f, 0.75f, 1.00f, 1.00f },
		{ 0.00f, 0.50f, 1.00f, 1.00f },	// sky
		{ 0.00f, 0.25f, 1.00f, 1.00f },
		{ 0.00f, 0.00f, 1.00f, 1.00f },	// blue
		{ 0.25f, 0.00f, 1.00f, 1.00f },
		{ 0.50f, 0.00f, 1.00f, 1.00f },	// purple
		{ 0.75f, 0.00f, 1.00f, 1.00f },
		{ 1.00f, 0.00f, 1.00f, 1.00f },	// magenta
		{ 1.00f, 0.00f, 0.75f, 1.00f },
		{ 1.00f, 0.00f, 0.50f, 1.00f },	// rose
		{ 1.00f, 0.00f, 0.25f, 1.00f },
	};
	const a3vec4 grey4[] = {
		{ 0.5f, 0.5f, 0.5f, 1.0f },	// solid grey
		{ 0.5f, 0.5f, 0.5f, 0.5f },	// translucent grey
	};
	const a3real
		* const red = rgba4[0].v, * const orange = rgba4[2].v, * const yellow = rgba4[4].v, * const lime = rgba4[6].v,
		* const green = rgba4[8].v, * const aqua = rgba4[10].v, * const cyan = rgba4[12].v, * const sky = rgba4[14].v,
		* const blue = rgba4[16].v, * const purple = rgba4[18].v, * const magenta = rgba4[20].v, * const rose = rgba4[22].v,
		* const grey = grey4[0].v, * const grey_t = grey4[1].v;
	const a3ui32 hueCount = sizeof(rgba4) / sizeof(*rgba4);

	// camera used for drawing
	const a3_SceneProjector* activeCamera = scene->projector + scene->activeCamera;
	const a3_SceneObject* activeCameraObject = activeCamera->sceneObject;

	// current hull for scene object being rendered, for convenience
	/*const a3_SceneObject* currentSceneObject, * endSceneObject;*/

	// temp drawable pointers
	const a3_VertexDrawable* drawable[] = {
		0,
		demoState->draw_node,
		demoState->draw_node,
		demoState->draw_unit_box,		// skybox

		demoState->draw_node,			// room root
		demoState->draw_unit_box,       // boxes
		demoState->draw_unit_box,       // 
		demoState->draw_unit_sphere,    // spheres
		demoState->draw_unit_sphere,    // 
		demoState->draw_unit_sphere,    // light
		demoState->draw_unit_box,       // room

		demoState->draw_node,           // materials root
		demoState->draw_unit_sphere,    // material balls
		demoState->draw_unit_sphere,    // 
		demoState->draw_unit_sphere,    // 
	};

	// temp texture pointers
	const a3_Texture* texture_dm[] = {
		0,
		0,
		0,
		demoState->tex_checker,			// skybox

		0,                  			// room root
		demoState->tex_checker,			// boxes
		demoState->tex_checker,			// 
		demoState->tex_checker,			// spheres
		demoState->tex_checker,			// 
		demoState->tex_checker,			// light
		demoState->tex_checker,			// room

		0,                              // materials root
		demoState->tex_checker,			// materia balls
		demoState->tex_checker,			// 
		demoState->tex_checker,			// 

		demoState->tex_add,
	};

	// model inversion
	const a3boolean invert_model[] = {
		0,
		0,
		0,
		1,

		0,
		0,
		0,
		0,
		0,
		0,
		1,

		0,
		0,
		0,
		0,
	};

	// program override
	const a3_SceneShaderProgram* render_program_override[] = {
		0,
		0,
		0,
		0,

		0,
		0,
		0,
		0,
		0,
		0,
		0,

		0,
		demoState->prog_drawPhotorealistic0,
		demoState->prog_drawPhotorealistic1,
		demoState->prog_drawPhotorealistic2,
	};

	// texture sets
	const a3_Texture* texture_set[][8] = {
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },

		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },

		{ 0 },
		{ demoState->tex_earth_dm, demoState->tex_earth_sm, demoState->tex_earth_nm, demoState->tex_earth_hm, demoState->tex_earth_cloud, demoState->tex_earth_light, demoState->tex_checker, 0 },
		{ demoState->tex_earth_dm, demoState->tex_earth_sm, demoState->tex_earth_nm, demoState->tex_earth_hm, demoState->tex_earth_cloud, demoState->tex_earth_light, demoState->tex_checker, 0 },
		{ demoState->tex_earth_dm, demoState->tex_earth_sm, demoState->tex_earth_nm, demoState->tex_earth_hm, demoState->tex_earth_cloud, demoState->tex_earth_light, demoState->tex_checker, 0 },
	};
	a3ui32 const max_texture_set_size = sizeof(*texture_set) / sizeof(**texture_set);

	// forward pipeline shader programs
	const a3_SceneShaderProgram* renderProgram[rendering_pipeline_max][rendering_render_max] = {
		{
			demoState->prog_drawColorUnif,
			demoState->prog_drawTexture,
			demoState->prog_drawLambert,
			demoState->prog_drawPhong,
			demoState->prog_drawRT,
		},
	};

	// display shader programs
	const a3_SceneShaderProgram* displayProgram[rendering_display_max] = {
		demoState->prog_drawTexture,
	};

	// framebuffers to which to write based on pipeline mode
	const a3_Framebuffer* writeFBO[rendering_pass_max] = {
		demoState->fbo_scene_c16d24s8_mrt,
		demoState->fbo_composite_c16,
		demoState->fbo_current_density_c16,
		demoState->fbo_prev_density_c16,
		demoState->fbo_current_velocity_c16,
		demoState->fbo_prev_velocity_c16,
		demoState->fbo_pressure_div_c16,
		demoState->fbo_tmp_buffer_c16,
		demoState->fbo_double_buffer_c16,
	};

	// framebuffers from which to read based on pipeline mode
	const a3_Framebuffer* readFBO[rendering_pass_max][4] = {
		{ 0, },
		{ demoState->fbo_scene_c16d24s8_mrt, },
	};

	// target info
	a3_Scene_Rendering_RenderProgramName const render = scene->render;
	a3_Scene_Rendering_DisplayProgramName const display = scene->display;
	a3_Scene_Rendering_PipelineName const pipeline = scene->pipeline;
	a3_Scene_Rendering_PassName const pass = scene->pass;
	a3_Scene_Rendering_TargetName const targetIndex = scene->targetIndex[pass], targetCount = scene->targetCount[pass];
	a3_Scene_Rendering_PassName currentPass;

	// FSQ matrix
	const a3mat4 fsq = {
		2.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 2.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 2.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
	};

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

	// final model matrix and full matrix stack
	a3mat4 projectionMat = activeCamera->projectionMat;
	a3mat4 projectionMatInv = activeCamera->projectionMatInv;
	a3mat4 viewMat = scene->sceneGraphState->objectSpaceInv->hpose_base[activeCameraObject->sceneGraphIndex].transformMat;
	a3mat4 viewProjectionMat;
	a3mat4 projectionBiasMat, projectionBiasMat_inv;
	a3mat4 modelMat, modelViewProjectionMat/*, modelViewMat*/;
	a3vec4 pixelSizeAndInv;

	// init
	a3real4x4Product(viewProjectionMat.m, projectionMat.m, viewMat.m);
	a3real4x4Product(projectionBiasMat.m, bias.m, projectionMat.m);
	a3real4x4Product(projectionBiasMat_inv.m, projectionMatInv.m, unbias.m);


	//-------------------------------------------------------------------------
	// 0) PRE-SCENE PASS: shadow pass renders scene to depth-only
	//	- activate shadow pass framebuffer
	//	- draw scene
	//		- clear depth buffer
	//		- render shapes using appropriate shaders
	//		- capture depth


	//-------------------------------------------------------------------------
	// 1) SCENE PASS: render scene with desired shader
	//	- activate scene framebuffer
	//	- draw scene
	//		- clear buffers
	//		- render shapes using appropriate shaders
	//		- capture color and depth

	// select target framebuffer
	currentPass = rendering_passScene;
	currentWriteFBO = writeFBO[currentPass];
	switch (pipeline)
	{
		// shading with MRT
	case rendering_forward:
		// target scene framebuffer
		a3scene_setSceneState(currentWriteFBO, demoState->displaySkybox);
		break;
	}


	// optional stencil test before drawing objects
	//a3real4x4SetScale(modelMat.m, a3real_four);
	//if (demoState->stencilTest)
	//	a3scene_drawStencilTest(modelViewProjectionMat.m, viewProjectionMat.m, modelMat.m, demoState->prog_drawColorUnif, demoState->draw_unit_sphere);


	// select program based on settings
	currentDemoProgram = renderProgram[pipeline][render];
	a3shaderProgramActivate(currentDemoProgram->program);

	// send shared data: 
	//	- projection matrix
	//	- light data
	//	- activate shared textures including atlases if using
	//	- shared animation data
	a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uP, 1, projectionMat.mm);
	a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uP_inv, 1, projectionMatInv.mm);
	a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uPB, 1, projectionBiasMat.mm);
	a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uPB_inv, 1, projectionBiasMat_inv.mm);
	a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uAtlas, 1, a3mat4_identity.mm);
	a3shaderUniformSendFloat(a3unif_vec4, currentDemoProgram->uColor, hueCount, rgba4->v);
	if (demoState->updateAnimation)
		a3shaderUniformSendDouble(a3unif_single, currentDemoProgram->uTime, 1, &demoState->timer_display->totalTime);

	// send target dimensions
	pixelSizeAndInv.x = (a3f32)currentWriteFBO->frameWidth;
	pixelSizeAndInv.y = (a3f32)currentWriteFBO->frameHeight;
	pixelSizeAndInv.z = 1.0f / pixelSizeAndInv.x;
	pixelSizeAndInv.w = 1.0f / pixelSizeAndInv.y;
	a3shaderUniformSendFloat(a3unif_vec4, currentDemoProgram->uAxis, 1, pixelSizeAndInv.v);

	glDisable(GL_BLEND);


	//-------------------------------------------------------------------------
	// COMPOSITE PASS
	//	- activate composite framebuffer
	//	- composite scene layers

	currentPass = rendering_passComposite;
	currentWriteFBO = writeFBO[currentPass];
	a3framebufferActivate(currentWriteFBO);

	// composite skybox
	currentDemoProgram = demoState->displaySkybox ? demoState->prog_drawTexture : demoState->prog_drawColorUnif;
	modelMat = scene->sceneGraphState->objectSpace->hpose_base[scene->obj_skybox->sceneGraphIndex].transformMat;
	a3scene_drawModelTexturedColored_invertModel(modelViewProjectionMat.m, viewProjectionMat.m, modelMat.m, a3mat4_identity.m, currentDemoProgram, demoState->draw_unit_box, demoState->tex_skybox_clouds, a3vec4_one.v);
	a3scene_enableCompositeBlending();

	// draw textured quad with previous pass image on it
	// repeat as necessary to complete composite
	currentDrawable = demoState->draw_unit_plane_z;
	a3vertexDrawableActivate(currentDrawable);

	switch (pipeline)
	{
	case rendering_forward:
		// use simple texturing program
		currentDemoProgram = demoState->prog_drawTexture;
		a3shaderProgramActivate(currentDemoProgram->program);
		// scene (color)
		currentReadFBO = readFBO[currentPass][0];
		a3framebufferBindColorTexture(currentReadFBO, a3tex_unit00, 0);
		break;
	}
	// reset other uniforms
	a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, fsq.mm);
	a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uAtlas, 1, a3mat4_identity.mm);
	a3shaderUniformSendFloat(a3unif_vec4, currentDemoProgram->uColor, 1, a3vec4_one.v);
	a3vertexDrawableRenderActive();


	//-------------------------------------------------------------------------
	// PREPARE FOR POST-PROCESSING
	//	- double buffer swap (if applicable)
	//	- ensure blending is disabled
	//	- re-activate FSQ drawable IF NEEDED (i.e. changed in previous step)
	glDisable(GL_BLEND);
	currentDrawable = demoState->draw_unit_plane_z;
	a3vertexDrawableActivate(currentDrawable);


	//-------------------------------------------------------------------------
	// POST-PROCESSING
	//	- activate target framebuffer
	//	- activate texture from previous framebuffer
	//	- draw FSQ with processing program active

	//test for border
	//currentDemoProgram = demoState->prog_drawTexture;
	//a3shaderProgramActivate(currentDemoProgram->program);
	//a3framebufferActivate(writeFBO[rendering_doubleBuffer]);

	//a3textureActivate(demoState->tex_earth_dm, a3tex_unit00);
	//a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, fsq.mm);
	//a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uAtlas, 1, a3mat4_identity.mm);

	//a3vertexDrawableRenderActive();

	//copy currents to prevs

	//velocity
	//1. add source (randomly generated vectors for now)
	a3f32 timeStep = (a3f32)0.016;
	a3f32 time = (a3f32)demoState->t_timer;

	if (demoState->first)
	{
		demoState->first = false;
		currentDemoProgram = demoState->prog_addForceVelocity;
		a3shaderProgramActivate(currentDemoProgram->program);
		a3framebufferActivate(writeFBO[rendering_doubleBuffer]);

		a3shaderUniformSendFloat(a3unif_single, a3shaderUniformGetLocation(currentDemoProgram->program, "uTimeStep"), 1, &timeStep);
		a3shaderUniformSendFloat(a3unif_single, a3shaderUniformGetLocation(currentDemoProgram->program, "uDt"), 1, &time);
		a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, fsq.mm);
		a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uAtlas, 1, a3mat4_identity.mm);
		a3textureActivate(texture_dm[15], a3tex_unit00);
		a3textureActivate(texture_dm[15], a3tex_unit01);

		a3vertexDrawableRenderActive();

		writeFBO[rendering_tmpBuffer] = writeFBO[rendering_prevVelocity];
		writeFBO[rendering_prevVelocity] = writeFBO[rendering_doubleBuffer];
		writeFBO[rendering_doubleBuffer] = writeFBO[rendering_tmpBuffer];

	}

	currentDemoProgram = demoState->prog_addForceVelocity;
	a3shaderProgramActivate(currentDemoProgram->program);
	a3framebufferActivate(writeFBO[rendering_doubleBuffer]);


	a3shaderUniformSendFloat(a3unif_single, a3shaderUniformGetLocation(currentDemoProgram->program, "uTimeStep"), 1, &timeStep);
	a3shaderUniformSendFloat(a3unif_single, a3shaderUniformGetLocation(currentDemoProgram->program, "uDt"), 1, &time);
	a3framebufferBindColorTexture(writeFBO[rendering_prevVelocity], a3tex_unit00, 0);
	a3textureActivate(texture_dm[15], a3tex_unit01);
	a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, fsq.mm);
	a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uAtlas, 1, a3mat4_identity.mm);

	a3vertexDrawableRenderActive();

	writeFBO[rendering_tmpBuffer] = writeFBO[rendering_currentVelocity];
	writeFBO[rendering_currentVelocity] = writeFBO[rendering_doubleBuffer];
	writeFBO[rendering_doubleBuffer] = writeFBO[rendering_tmpBuffer];

	////2. swap prev and current
	writeFBO[rendering_tmpBuffer] = writeFBO[rendering_prevVelocity];
	writeFBO[rendering_prevVelocity] = writeFBO[rendering_currentVelocity];
	writeFBO[rendering_currentVelocity] = writeFBO[rendering_tmpBuffer];

	//2. diffuse (jacobi) x20
	//		-set bounds
	for (int i = 0; i < 20; i++)
	{
		currentDemoProgram = demoState->prog_jacobiDiffuse;
		a3shaderProgramActivate(currentDemoProgram->program);
		a3framebufferActivate(writeFBO[rendering_doubleBuffer]);

		currentDrawable = demoState->draw_unit_plane_z;
		a3vertexDrawableActivate(currentDrawable);

		a3framebufferBindColorTexture(writeFBO[rendering_prevVelocity], a3tex_unit00, 0);
		a3framebufferBindColorTexture(writeFBO[rendering_currentVelocity], a3tex_unit01, 0);

		a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, fsq.mm);
		a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uAtlas, 1, a3mat4_identity.mm);

		a3f32 a = (a3f32)(timeStep * 0.2 * 800 * 800);
		a3f32 diffuseDenom = (a3f32)(1.0 / (4.0 * a + 1.0));
		a3shaderUniformSendFloat(a3unif_single, a3shaderUniformGetLocation(currentDemoProgram->program, "a"), 1, &a);
		a3shaderUniformSendFloat(a3unif_single, a3shaderUniformGetLocation(currentDemoProgram->program, "diffuseDenom"), 1, &diffuseDenom);

		a3vertexDrawableRenderActive();
		writeFBO[rendering_tmpBuffer] = writeFBO[rendering_currentVelocity];
		writeFBO[rendering_currentVelocity] = writeFBO[rendering_doubleBuffer];
		writeFBO[rendering_doubleBuffer] = writeFBO[rendering_tmpBuffer];

		bounds(demoState, targetIndex, writeFBO[rendering_currentVelocity], writeFBO[rendering_doubleBuffer], fsq);

		writeFBO[rendering_tmpBuffer] = writeFBO[rendering_currentVelocity];
		writeFBO[rendering_currentVelocity] = writeFBO[rendering_doubleBuffer];
		writeFBO[rendering_doubleBuffer] = writeFBO[rendering_tmpBuffer];

		//swap prev and current
		writeFBO[rendering_tmpBuffer] = writeFBO[rendering_prevVelocity];
		writeFBO[rendering_prevVelocity] = writeFBO[rendering_currentVelocity];
		writeFBO[rendering_currentVelocity] = writeFBO[rendering_tmpBuffer];
	}



	//3. project
			//-divergence
	currentDemoProgram = demoState->prog_div;
	a3shaderProgramActivate(currentDemoProgram->program);
	a3framebufferActivate(writeFBO[rendering_pressureDiv]);
	a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, fsq.mm);
	a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uAtlas, 1, a3mat4_identity.mm);

	currentDrawable = demoState->draw_unit_plane_z;
	a3vertexDrawableActivate(currentDrawable);

	a3framebufferBindColorTexture(writeFBO[rendering_currentVelocity], a3tex_unit00, 0);
	//a3textureActivate(demoState->tex_earth_dm, a3tex_unit00);

	a3vertexDrawableRenderActive();

	////		-set pressure and divergence bounds (happens automatically due to clamping)
	////		-jacobi x20 
	////			-set pressure bounds
	//for (int i = 0; i < 20; i++)
	//{
	//	currentDemoProgram = demoState->prog_jacobiProject;
	//	a3shaderProgramActivate(currentDemoProgram->program);
	//	a3framebufferActivate(writeFBO[rendering_doubleBuffer]);

	//	a3framebufferBindColorTexture(writeFBO[rendering_pressureDiv], a3tex_unit00, 0);

	//	a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, fsq.mm);
	//	a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uAtlas, 1, a3mat4_identity.mm);

	//	a3vertexDrawableRenderActive();
	//	writeFBO[rendering_tmpBuffer] = writeFBO[rendering_pressureDiv];
	//	writeFBO[rendering_pressureDiv] = writeFBO[rendering_doubleBuffer];
	//	writeFBO[rendering_doubleBuffer] = writeFBO[rendering_tmpBuffer];

	//	bounds(demoState, targetIndex, writeFBO[rendering_pressureDiv], writeFBO[rendering_doubleBuffer], fsq, (a3f32)1.0);

	//	writeFBO[rendering_tmpBuffer] = writeFBO[rendering_pressureDiv];
	//	writeFBO[rendering_pressureDiv] = writeFBO[rendering_doubleBuffer];
	//	writeFBO[rendering_doubleBuffer] = writeFBO[rendering_tmpBuffer];
	//}

	//		-gradient
	//currentDemoProgram = demoState->prog_gradient;
	//a3shaderProgramActivate(currentDemoProgram->program);
	//a3framebufferActivate(writeFBO[rendering_doubleBuffer]);

	//a3framebufferBindColorTexture(writeFBO[rendering_currentVelocity], a3tex_unit00, 0);
	//a3framebufferBindColorTexture(writeFBO[rendering_pressureDiv], a3tex_unit01, 0);

	//a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, fsq.mm);
	//a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uAtlas, 1, a3mat4_identity.mm);

	//a3vertexDrawableRenderActive();
	//writeFBO[rendering_tmpBuffer] = writeFBO[rendering_currentVelocity];
	//writeFBO[rendering_currentVelocity] = writeFBO[rendering_doubleBuffer];
	//writeFBO[rendering_doubleBuffer] = writeFBO[rendering_tmpBuffer];

	////		-set velocity bounds
	///*currentDemoProgram = demoState->prog_bounds;
	//a3shaderProgramActivate(currentDemoProgram->program);
	//a3framebufferActivate(writeFBO[rendering_currentVelocity]);

	////		-set velocity bounds
	//bounds(demoState, targetIndex, writeFBO[rendering_currentVelocity], writeFBO[rendering_doubleBuffer], fsq, (a3f32)1.0);

	////4. swap
	//writeFBO[rendering_tmpBuffer] = writeFBO[rendering_prevVelocity];
	//writeFBO[rendering_prevVelocity] = writeFBO[rendering_doubleBuffer];
	//writeFBO[rendering_doubleBuffer] = writeFBO[rendering_tmpBuffer];

	//////5. advect
	//
	//currentDemoProgram = demoState->prog_advect;
	//a3shaderProgramActivate(currentDemoProgram->program);
	//a3framebufferActivate(writeFBO[rendering_doubleBuffer]);

	//a3framebufferBindColorTexture(writeFBO[rendering_prevVelocity], a3tex_unit00, 0);
	//a3framebufferBindColorTexture(writeFBO[rendering_currentVelocity], a3tex_unit01, 0);

	//a3shaderUniformSendFloat(a3unif_single, a3shaderUniformGetLocation(currentDemoProgram->program, "dt"), 1, &timeStep);
	//a3f32 y = (a3f32)800;
	//a3shaderUniformSendFloat(a3unif_single, a3shaderUniformGetLocation(currentDemoProgram->program, "N"), 1, &y);

	//a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, fsq.mm);
	//a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uAtlas, 1, a3mat4_identity.mm);

	//a3vertexDrawableRenderActive();
	//writeFBO[rendering_tmpBuffer] = writeFBO[rendering_currentVelocity];
	//writeFBO[rendering_currentVelocity] = writeFBO[rendering_doubleBuffer];
	//writeFBO[rendering_doubleBuffer] = writeFBO[rendering_tmpBuffer];

	////		-set bounds
	//bounds(demoState, targetIndex, writeFBO[rendering_currentVelocity], writeFBO[rendering_doubleBuffer], fsq, (a3f32)1.0);

	////4. swap
	//writeFBO[rendering_tmpBuffer] = writeFBO[rendering_currentVelocity];
	//writeFBO[rendering_currentVelocity] = writeFBO[rendering_doubleBuffer];
	//writeFBO[rendering_doubleBuffer] = writeFBO[rendering_tmpBuffer];

	////6. project steps again
	////		-divergence
	//currentDemoProgram = demoState->prog_divergence;
	//a3shaderProgramActivate(currentDemoProgram->program);
	//a3framebufferActivate(writeFBO[rendering_pressureDiv]);
	//a3framebufferBindColorTexture(writeFBO[rendering_currentVelocity], a3tex_unit00, 0);

	//a3vertexDrawableRenderActive();

	////		-set pressure and divergence bounds (happens automatically due to clamping)
	////		-jacobi x20 
	////			-set pressure bounds
	//for (int i = 0; i < 20; i++)
	//{
	//	currentDemoProgram = demoState->prog_jacobiProject;
	//	a3shaderProgramActivate(currentDemoProgram->program);
	//	a3framebufferActivate(writeFBO[rendering_doubleBuffer]);

	//	a3framebufferBindColorTexture(writeFBO[rendering_pressureDiv], a3tex_unit00, 0);

	//	a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, fsq.mm);
	//	a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uAtlas, 1, a3mat4_identity.mm);

	//	a3vertexDrawableRenderActive();
	//	writeFBO[rendering_tmpBuffer] = writeFBO[rendering_pressureDiv];
	//	writeFBO[rendering_pressureDiv] = writeFBO[rendering_doubleBuffer];
	//	writeFBO[rendering_doubleBuffer] = writeFBO[rendering_tmpBuffer];

	//	bounds(demoState, targetIndex, writeFBO[rendering_pressureDiv], writeFBO[rendering_doubleBuffer], fsq, (a3f32)1.0);

	//	writeFBO[rendering_tmpBuffer] = writeFBO[rendering_pressureDiv];
	//	writeFBO[rendering_pressureDiv] = writeFBO[rendering_doubleBuffer];
	//	writeFBO[rendering_doubleBuffer] = writeFBO[rendering_tmpBuffer];
	//}

	//		-gradient
	//currentDemoProgram = demoState->prog_gradient;
	//a3shaderProgramActivate(currentDemoProgram->program);
	//a3framebufferActivate(writeFBO[rendering_doubleBuffer]);

	//a3framebufferBindColorTexture(writeFBO[rendering_currentVelocity], a3tex_unit00, 0);
	//a3framebufferBindColorTexture(writeFBO[rendering_pressureDiv], a3tex_unit01, 0);

	//a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, fsq.mm);
	//a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uAtlas, 1, a3mat4_identity.mm);

	//a3vertexDrawableRenderActive();
	//writeFBO[rendering_tmpBuffer] = writeFBO[rendering_currentVelocity];
	//writeFBO[rendering_currentVelocity] = writeFBO[rendering_doubleBuffer];
	//writeFBO[rendering_doubleBuffer] = writeFBO[rendering_tmpBuffer];

	//a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, fsq.mm);
	//a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uAtlas, 1, a3mat4_identity.mm);

	////		-set velocity bounds
	//bounds(demoState, targetIndex, writeFBO[rendering_currentVelocity], writeFBO[rendering_doubleBuffer], fsq, (a3f32)1.0);

	////density
	////1. add source (from texture)
	//currentDemoProgram = demoState->prog_addForceVelocity;
	//a3shaderProgramActivate(currentDemoProgram->program);
	//a3framebufferActivate(writeFBO[rendering_currentDensity]);

	//a3framebufferBindColorTexture(writeFBO[rendering_currentDensity], a3tex_unit00, 0);
	//a3textureActivate(texture_dm[16], a3tex_unit01);

	//a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, fsq.mm);
	//a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uAtlas, 1, a3mat4_identity.mm);
	//a3shaderUniformSendFloat(a3unif_single, a3shaderUniformGetLocation(currentDemoProgram->program, "dt"), 1, &x);

	//a3vertexDrawableRenderActive();
	//writeFBO[rendering_tmpBuffer] = writeFBO[rendering_currentDensity];
	//writeFBO[rendering_currentDensity] = writeFBO[rendering_doubleBuffer];
	//writeFBO[rendering_doubleBuffer] = writeFBO[rendering_tmpBuffer];

	////2. swap
	//writeFBO[rendering_tmpBuffer] = writeFBO[rendering_prevDensity];
	//writeFBO[rendering_prevDensity] = writeFBO[rendering_currentDensity];
	//writeFBO[rendering_currentDensity] = writeFBO[rendering_tmpBuffer];

	////3. diffuse (jacobi) x20
	//for (int i = 0; i < 20; i++)
	//{
	//	currentDemoProgram = demoState->prog_jacobiDiffuse;
	//	a3shaderProgramActivate(currentDemoProgram->program);
	//	a3framebufferActivate(writeFBO[rendering_doubleBuffer]);

	//	a3framebufferBindColorTexture(writeFBO[rendering_prevDensity], a3tex_unit00, 0);
	//	a3framebufferBindColorTexture(writeFBO[rendering_currentDensity], a3tex_unit01, 0);

	//	a3f32 a = (a3f32)(x * 0.2 * 800 * 800);
	//	a3f32 diffuseDenom = (a3f32)(1.0 / (4.0 * a + 1.0));
	//	a3shaderUniformSendFloat(a3unif_single, a3shaderUniformGetLocation(currentDemoProgram->program, "a"), 1, &a);
	//	a3shaderUniformSendFloat(a3unif_single, a3shaderUniformGetLocation(currentDemoProgram->program, "diffuseDenom"), 1, &diffuseDenom);

	//	a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, fsq.mm);
	//	a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uAtlas, 1, a3mat4_identity.mm);

	//	a3vertexDrawableRenderActive();
	//	writeFBO[rendering_tmpBuffer] = writeFBO[rendering_currentDensity];
	//	writeFBO[rendering_currentDensity] = writeFBO[rendering_doubleBuffer];
	//	writeFBO[rendering_doubleBuffer] = writeFBO[rendering_tmpBuffer];

	//	/*currentDemoProgram = demoState->prog_bounds;
	//	a3shaderProgramActivate(currentDemoProgram->program);
	//	a3framebufferActivate(writeFBO[rendering_currentDensity]);

	//	a3framebufferBindColorTexture(writeFBO[rendering_currentDensity], a3tex_unit00, 0);
	//	a3textureActivate(texture_dm[15], a3tex_unit01);

	//	a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, fsq.mm);
	//	a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uAtlas, 1, a3mat4_identity.mm);

	//	a3vertexDrawableRenderActive();*/
	//}

	////4. advect
	////		-bounds
	//currentDemoProgram = demoState->prog_advect;
	//a3shaderProgramActivate(currentDemoProgram->program);
	//a3framebufferActivate(writeFBO[rendering_currentDensity]);

	//a3framebufferBindColorTexture(writeFBO[rendering_prevDensity], a3tex_unit00, 0);
	//a3framebufferBindColorTexture(writeFBO[rendering_currentVelocity], a3tex_unit01, 0);

	//a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, fsq.mm);
	//a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uAtlas, 1, a3mat4_identity.mm);
	//a3shaderUniformSendFloat(a3unif_single, a3shaderUniformGetLocation(currentDemoProgram->program, "dt"), 1, &x);
	//a3shaderUniformSendFloat(a3unif_single, a3shaderUniformGetLocation(currentDemoProgram->program, "N"), 1, &y);

	//a3vertexDrawableRenderActive();

	///*currentDemoProgram = demoState->prog_bounds;
	//a3shaderProgramActivate(currentDemoProgram->program);
	//a3framebufferActivate(writeFBO[rendering_currentDensity]);

	//a3framebufferBindColorTexture(writeFBO[rendering_currentDensity], a3tex_unit00, 0);
	//a3textureActivate(texture_dm[15], a3tex_unit01);

	//a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, fsq.mm);
	//a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uAtlas, 1, a3mat4_identity.mm);

	//a3vertexDrawableRenderActive();*/

	////fade
	//currentDemoProgram = demoState->prog_fade;
	//a3shaderProgramActivate(currentDemoProgram->program);
	//a3framebufferActivate(writeFBO[rendering_doubleBuffer]);

	//a3framebufferBindColorTexture(writeFBO[rendering_currentDensity], a3tex_unit00, 0);

	//a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, fsq.mm);
	//a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uAtlas, 1, a3mat4_identity.mm);
	//a3shaderUniformSendFloat(a3unif_single, a3shaderUniformGetLocation(currentDemoProgram->program, "dt"), 1, &x);
	//a3f32 f = (a3f32)0.00005;
	//a3shaderUniformSendFloat(a3unif_single, a3shaderUniformGetLocation(currentDemoProgram->program, "fade"), 1, &f);

	//a3vertexDrawableRenderActive();
	//writeFBO[rendering_tmpBuffer] = writeFBO[rendering_currentDensity];
	//writeFBO[rendering_currentDensity] = writeFBO[rendering_doubleBuffer];
	//writeFBO[rendering_doubleBuffer] = writeFBO[rendering_tmpBuffer];

	//-------------------------------------------------------------------------
	// DISPLAY: final pass, perform and present final composite
	//	- finally draw to back buffer
	//	- select display texture(s)
	//	- activate final pass program
	//	- draw final FSQ

	// revert to back buffer and disable depth testing
	a3framebufferDeactivateSetViewport(a3fbo_depthDisable,
		-demoState->frameBorder, -demoState->frameBorder, demoState->frameWidth, demoState->frameHeight);

	// select framebuffer to display based on mode
	currentDisplayFBO = writeFBO[scene->pass];

	// select output to display
	switch (scene->pass)
	{
	case rendering_passScene:
		if (currentDisplayFBO->color && (!currentDisplayFBO->depthStencil || targetIndex < targetCount - 1))
			a3framebufferBindColorTexture(currentDisplayFBO, a3tex_unit00, targetIndex);
		else
			a3framebufferBindDepthTexture(currentDisplayFBO, a3tex_unit00);
		break;
	case rendering_passComposite:
		a3framebufferBindColorTexture(currentDisplayFBO, a3tex_unit00, targetIndex);
		break;
	}

	// final display: activate desired final program and draw FSQ
	if (currentDisplayFBO)
	{
		//testing
		//bounds(demoState, targetIndex, writeFBO[rendering_doubleBuffer], writeFBO[rendering_currentVelocity], fsq);

		//copy buffer to demostate framebuffer
		if (demoState->fbo_prev_velocity_c16 != writeFBO[rendering_prevVelocity])
		{
			currentDemoProgram = demoState->prog_drawTexture;
			a3shaderProgramActivate(currentDemoProgram->program);
			a3framebufferActivate(demoState->fbo_prev_velocity_c16);

			//read
			a3framebufferBindColorTexture(writeFBO[rendering_prevVelocity], a3tex_unit00, 0);

			currentDrawable = demoState->draw_unit_plane_z;
			a3vertexDrawableActivate(currentDrawable);

			// done
			a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, fsq.mm);
			a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uAtlas, 1, a3mat4_identity.mm);
			a3shaderUniformSendFloat(a3unif_vec4, currentDemoProgram->uColor, 1, a3vec4_one.v);


			a3vertexDrawableRenderActive();
		}

		if (demoState->fbo_current_velocity_c16 != writeFBO[rendering_currentVelocity])
		{
			currentDemoProgram = demoState->prog_drawTexture;
			a3shaderProgramActivate(currentDemoProgram->program);
			a3framebufferActivate(demoState->fbo_current_velocity_c16);

			//read
				a3framebufferBindColorTexture(writeFBO[rendering_currentVelocity], a3tex_unit00, 0);

			currentDrawable = demoState->draw_unit_plane_z;
			a3vertexDrawableActivate(currentDrawable);

			//done
				a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, fsq.mm);
			a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uAtlas, 1, a3mat4_identity.mm);
			a3shaderUniformSendFloat(a3unif_vec4, currentDemoProgram->uColor, 1, a3vec4_one.v);


			a3vertexDrawableRenderActive();
		}

		////---------------------------FINAL DRAW---------------------------
		a3framebufferDeactivateSetViewport(a3fbo_depthDisable,
			-demoState->frameBorder, -demoState->frameBorder, demoState->frameWidth, demoState->frameHeight);
		currentDrawable = demoState->draw_unit_plane_z;
		currentDemoProgram = demoState->prog_drawTexture;
		a3vertexDrawableActivate(currentDrawable);
		a3shaderProgramActivate(currentDemoProgram->program);
		a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, fsq.mm);
		a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uAtlas, 1, a3mat4_identity.mm);
		a3shaderUniformSendFloat(a3unif_vec4, currentDemoProgram->uColor, 1, a3vec4_one.v);
		a3framebufferBindColorTexture(writeFBO[rendering_pressureDiv], a3tex_unit00, 0);
		a3vertexDrawableRenderActive(); 
	}
}


//-----------------------------------------------------------------------------