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
	
	a3_SceneRenderUtils.c
	Drawing utilities implementations.
*/

#include "../a3_SceneRenderUtils.h"
#include "../a3_SceneObject.h"

#include "A3_DEMO/_animation/a3_HierarchyState.h"


//-----------------------------------------------------------------------------

// OpenGL
#ifdef _WIN32
#include <Windows.h>
#include <GL/GL.h>
#else	// !_WIN32
#include <OpenGL/gl3.h>
#endif	// _WIN32


//-----------------------------------------------------------------------------
// GENERAL SETUP AND STATE CHANGE UTILITIES

// blending state for composition
extern inline void a3scene_enableCompositeBlending()
{
	// result = ( new*[new alpha] ) + ( old*[1 - new alpha] )
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

// blending state for accumulation
extern inline void a3scene_enableAdditiveBlending()
{
	// result = ( new*[1] ) + ( old*[1] )
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
}

// default stencil state for writing
extern inline void a3scene_enableStencilWrite()
{
	glStencilFunc(GL_ALWAYS, 1, 0xff);			// any stencil value will be set to 1
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);	// replace stencil value if S&D tests pass
	glStencilMask(0xff);						// write to stencil buffer
}

// default stencil state for comparing
extern inline void a3scene_enableStencilCompare()
{
	glStencilFunc(GL_EQUAL, 1, 0xff);			// stencil test passes if equal to 1
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);		// keep stencil value if S&D tests pass
	glStencilMask(0x00);						// don't write to stencil buffer
}


// set default GL state
extern inline void a3scene_setDefaultGraphicsState()
{
	const a3f32 lineWidth = 2.0f;
	const a3f32 pointSize = 4.0f;

	// lines and points
	glLineWidth(lineWidth);
	glPointSize(pointSize);

	// backface culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// textures
	glEnable(GL_TEXTURE_2D);

	// background
	glClearColor(0.0f, 0.0f, 0.0, 0.0f);

	// alpha blending
	a3scene_enableCompositeBlending();
}

// set scene draw state
extern inline void a3scene_setSceneState(a3_Framebuffer const* currentWriteFBO, a3boolean displaySkybox)
{
	// activate FBO
	a3framebufferActivate(currentWriteFBO);

	// choose background color
//	if (displaySkybox)
//		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
//	else
//		glClearColor(0.1f, 0.1f, 0.6f, 1.0f);

	// clear now, handle skybox later
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_BLEND);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


//-----------------------------------------------------------------------------
// RENDER SUB-ROUTINES

extern inline void a3scene_drawModelSimple(a3real4x4p modelViewProjectionMat, a3real4x4p const viewProjectionMat, a3real4x4p const modelMat, a3_SceneShaderProgram const* program)
{
	a3real4x4Product(modelViewProjectionMat, viewProjectionMat, modelMat);
	a3shaderUniformSendFloatMat(a3unif_mat4, 0, program->uMVP, 1, *modelViewProjectionMat);
	a3vertexDrawableRenderActive();
}

extern inline void a3scene_drawModelSimple_activateModel(a3real4x4p modelViewProjectionMat, a3real4x4p const viewProjectionMat, a3real4x4p const modelMat, a3_SceneShaderProgram const* program, a3_VertexDrawable const* drawable)
{
	a3real4x4Product(modelViewProjectionMat, viewProjectionMat, modelMat);
	a3shaderUniformSendFloatMat(a3unif_mat4, 0, program->uMVP, 1, *modelViewProjectionMat);
	a3vertexDrawableActivateAndRender(drawable);
}

extern inline void a3scene_drawModelSolidColor(a3real4x4p modelViewProjectionMat, a3real4x4p const viewProjectionMat, a3real4x4p const modelMat, a3_SceneShaderProgram const* program, a3_VertexDrawable const* drawable, a3real4p const color)
{
	// set up render
	a3shaderProgramActivate(program->program);
	a3shaderUniformSendFloat(a3unif_vec4, program->uColor, 1, color);
	a3scene_drawModelSimple_activateModel(modelViewProjectionMat, viewProjectionMat, modelMat, program, drawable);
}

extern inline void a3scene_drawModelTexturedColored_invertModel(a3real4x4p modelViewProjectionMat, a3real4x4p const viewProjectionMat, a3real4x4p const modelMat, a3real4x4p const atlasMat, a3_SceneShaderProgram const* program, a3_VertexDrawable const* drawable, a3_Texture const* texture, a3real4p const color)
{
	// set up render
	a3shaderProgramActivate(program->program);
	a3shaderUniformSendFloatMat(a3unif_mat4, 0, program->uAtlas, 1, *atlasMat);
	a3shaderUniformSendFloat(a3unif_vec4, program->uColor, 1, color);
	a3textureActivate(texture, a3tex_unit00);

	// draw inverted
	glCullFace(GL_FRONT);
	a3scene_drawModelSimple_activateModel(modelViewProjectionMat, viewProjectionMat, modelMat, program, drawable);
	glCullFace(GL_BACK);
}

extern inline void a3scene_drawModelLighting(a3real4x4p modelViewProjectionMat, a3real4x4p modelViewMat, a3real4x4p const viewProjectionMat, a3real4x4p const viewMat, a3real4x4p const modelMat, a3_SceneShaderProgram const*program, a3_VertexDrawable const* drawable, a3real4p const color)
{
	// set up render
	a3real4x4Product(modelViewMat, viewMat, modelMat);
	a3shaderUniformSendFloatMat(a3unif_mat4, 0, program->uMV, 1, *modelViewMat);
	a3scene_quickInvertTranspose_internal(modelViewMat);
	a3real4SetReal4(modelViewMat[3], a3vec4_zero.v);
	a3shaderUniformSendFloatMat(a3unif_mat4, 0, program->uMV_nrm, 1, *modelViewMat);
	a3shaderUniformSendFloat(a3unif_vec4, program->uColor, 1, color);

	// draw
	a3scene_drawModelSimple_activateModel(modelViewProjectionMat, viewProjectionMat, modelMat, program, drawable);
}

extern inline void a3scene_drawModelLighting_bias_other(a3real4x4p modelViewProjectionBiasMat_other, a3real4x4p modelViewProjectionMat, a3real4x4p modelViewMat, a3real4x4p const viewProjectionBiasMat_other, a3real4x4p const viewProjectionMat, a3real4x4p const viewMat, a3real4x4p const modelMat, a3_SceneShaderProgram const* program, a3_VertexDrawable const* drawable, a3real4p const color)
{
	// set up render
	a3real4x4Product(modelViewMat, viewMat, modelMat);
	a3shaderUniformSendFloatMat(a3unif_mat4, 0, program->uMV, 1, *modelViewMat);
	a3scene_quickInvertTranspose_internal(modelViewMat);
	a3real4SetReal4(modelViewMat[3], a3vec4_zero.v);
	a3shaderUniformSendFloatMat(a3unif_mat4, 0, program->uMV_nrm, 1, *modelViewMat);
	a3shaderUniformSendFloat(a3unif_vec4, program->uColor, 1, color);
	a3real4x4Product(modelViewProjectionBiasMat_other, viewProjectionBiasMat_other, modelMat);
	a3shaderUniformSendFloatMat(a3unif_mat4, 0, program->uMVPB_other, 1, *modelViewProjectionBiasMat_other);

	// draw
	a3scene_drawModelSimple_activateModel(modelViewProjectionMat, viewProjectionMat, modelMat, program, drawable);
}

extern inline void a3scene_drawStencilTest(a3real4x4p modelViewProjectionMat, a3real4x4p const viewProjectionMat, a3real4x4p const modelMat, a3_SceneShaderProgram const* program, a3_VertexDrawable const* drawable)
{
	// draw to stencil buffer: 
	//	- render first sphere to the stencil buffer to set drawable area
	//		- don't want values for the shape to actually be drawn to 
	//			color or depth buffers, so apply a MASK for this object
	//	- enable stencil test for everything else

	// drawing "lens" object using simple program
	a3shaderProgramActivate(program->program);

	// default stencil write settings
	a3scene_enableStencilWrite();

	// enable test and clear buffer (do this after mask is set)
	glEnable(GL_STENCIL_TEST);
	glClear(GL_STENCIL_BUFFER_BIT);

	// disable drawing this object to color or depth
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_FALSE);

	// inverted small sphere in solid transparent color
	// used as our "lens" for the depth and stencil tests
	glCullFace(GL_FRONT);
	a3scene_drawModelSimple_activateModel(modelViewProjectionMat, viewProjectionMat, modelMat, program, drawable);
	glCullFace(GL_BACK);

	// enable drawing following objects to color and depth
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDepthMask(GL_TRUE);

	// default stencil compare settings
	a3scene_enableStencilCompare();
}

//inline a3i32 a3SSBOCreate(a3i32 uniformLocation)
//{
//	return glCreateBuffers(1, &uniformLocation);
//}
//
//inline a3ret a3SSBOSet(a3i32 uniformLocation, a3i64 bytes, const void* data)
//{
//	glNamedBufferStorage(uniformLocation, bytes, data);
//}


//-----------------------------------------------------------------------------

inline a3real4r a3demo_mat2quat_safe(a3real4 q, a3real4x4 const m)
{
	a3real r, s;
	a3real3x3 t;
	a3real3GetUnit(t[0], m[0]);
	a3real3GetUnit(t[1], m[1]);
	a3real3GetUnit(t[2], m[2]);

	s = t[0][0] + t[1][1] + t[2][2];
	if (s > a3real_zero)
	{
		r = a3real_half / a3sqrt(a3real_one + s);
		a3real4Set(q, r * (t[1][2] - t[2][1]), r * (t[2][0] - t[0][2]), r * (t[0][1] - t[1][0]), a3real_quarter / r);
	}
	else if (t[0][0] > t[1][1] && t[0][0] > t[2][2])
	{
		r = a3real_half / a3sqrt(a3real_one + t[0][0] - t[1][1] - t[2][2]);
		a3real4Set(q, a3real_quarter / r, r * (t[0][1] + t[1][0]), r * (t[2][0] + t[0][2]), r * (t[1][2] - t[2][1]));
	}
	else if (t[1][1] > t[2][2])
	{
		r = a3real_half / a3sqrt(a3real_one - t[0][0] + t[1][1] - t[2][2]);
		a3real4Set(q, r * (t[0][1] + t[1][0]), a3real_quarter / r, r * (t[1][2] + t[2][1]), r * (t[2][0] - t[0][2]));
	}
	else
	{
		r = a3real_half / a3sqrt(a3real_one - t[0][0] - t[1][1] + t[2][2]);
		a3real4Set(q, r * (t[2][0] + t[0][2]), r * (t[1][2] + t[2][1]), a3real_quarter / r, r * (t[0][1] - t[1][0]));
	}

	// flip if real part is negative
	if (q[3] < a3real_zero)
		a3real4Negate(q);

	// done
	return q;
}

inline a3real4x2r a3demo_mat2dquat_safe(a3real4x2 Q, a3real4x4 const m)
{
	a3real3 ht;
	a3real4r const r = Q[0], d = Q[1];
	a3real4rk const t = m[3];
	a3real3ProductS(ht, t, a3real_half);

	a3demo_mat2quat_safe(r, m);

	// 0.5 t r	= 0.5(t.w * r.w - dot(t.v, r.v), t.w * r.v + r.w * t.v + cross(t.v, r.v))
	//			= 0.5(-dot(t.v, r.v), r.w * t.v + cross(t.v, r.v))
	d[3] = -a3real3Dot(ht, r);
	a3real3Cross(d, ht, r);
	a3real3Add(d, a3real3MulS(ht, r[3]));

	// done
	return Q;
}


//-----------------------------------------------------------------------------

// integrate Euler
inline a3real a3demo_integrateEuler(a3real const x, a3real const dx_dt, a3real const dt)
{
	return (x + dx_dt * dt);
}

// integrate kinematic
inline a3real a3demo_integrateKinematic(a3real const x, a3real const dx_dt, a3real const d2x_dt2, a3real const dt)
{
	return (x + (dx_dt + d2x_dt2 * a3real_half * dt) * dt);
}

// integrate Euler for vec2
inline a3real2r a3demo_integrateEuler2(a3real2p y, a3real2p const x, a3real2p const dx_dt, a3real const dt)
{
	y[0] = a3demo_integrateEuler(x[0], dx_dt[0], dt);
	y[1] = a3demo_integrateEuler(x[1], dx_dt[1], dt);
	return y;
}

// integrate kinematic for vec2
inline a3real2r a3demo_integrateKinematic2(a3real2p y, a3real2p const x, a3real2p const dx_dt, a3real2p const d2x_dt2, a3real const dt)
{
	y[0] = a3demo_integrateKinematic(x[0], dx_dt[0], d2x_dt2[0], dt);
	y[1] = a3demo_integrateKinematic(x[1], dx_dt[1], d2x_dt2[1], dt);
	return y;
}

void a3demo_uploadTransformStacks(
    a3_UniformBuffer const* ubo_transform_stacks,
    a3_SceneModelMatrixStack const* model_matrix_stacks, a3_SceneViewerMatrixStack const* viewer_matrix_stacks,
    a3ui32 const max_models, a3ui32 const num_models, a3ui32 const max_viewers, a3ui32 const num_viewers)
{
    a3bufferFixedRefill(ubo_transform_stacks, 0, num_viewers * sizeof(a3_SceneViewerMatrixStack), viewer_matrix_stacks);
    a3bufferFixedRefillOffset(ubo_transform_stacks, 0, max_viewers * sizeof(a3_SceneViewerMatrixStack), num_models * sizeof(a3_SceneModelMatrixStack), model_matrix_stacks);
}

void a3demo_uploadHierarchyGraphics(
	a3_UniformBuffer const* ubo_transform_joints, a3_UniformBuffer const* ubo_transform_bones, a3_UniformBuffer const* ubo_transform_skin,
	a3mat4 const* transform_mvp_joints, a3mat4 const* transform_mvp_bones, a3mat4 const* transform_t_skin, a3dualquat const* transform_dq_skin,
	a3ui32 const max_transforms, a3ui32 const numNodes)
{
	a3ui32 const mvp_size = numNodes * sizeof(a3mat4);
	a3ui32 const t_skin_size = max_transforms * sizeof(a3mat4);
	a3ui32 const dq_skin_size = max_transforms * sizeof(a3dualquat);

	// upload
	a3bufferFixedRefill(ubo_transform_joints, 0, mvp_size, transform_mvp_joints);
	a3bufferFixedRefill(ubo_transform_bones, 0, mvp_size, transform_mvp_bones);
	a3bufferFixedRefill(ubo_transform_skin, 0, t_skin_size, transform_t_skin);
	a3bufferFixedRefillOffset(ubo_transform_skin, 0, t_skin_size, dq_skin_size, transform_dq_skin);
}

void a3demo_updateHierarchyGraphics(
	a3mat4* transform_mvp_joints, a3mat4* transform_mvp_bones, a3mat4* transform_t_skin, a3dualquat* transform_dq_skin,
	a3ui32 const max_transforms, a3mat4 const mvp_obj, a3_HierarchyState const* activeHS)
{
	a3mat4 scaleMat = a3mat4_identity;
	a3mat4* mvp_joint, * mvp_bone, * t_skin;
	a3dualquat* dq_skin;
	a3index i;
	a3i32 p;

	// update joint and bone transforms
	for (i = 0; i < activeHS->hierarchy->numNodes; ++i)
	{
		mvp_joint = &transform_mvp_joints[i];
		mvp_bone = &transform_mvp_bones[i];
		t_skin = &transform_t_skin[i];
		dq_skin = &transform_dq_skin[i];

		// joint transform
		a3real4x4SetScale(scaleMat.m, a3real_sixth);
		a3real4x4Concat(activeHS->objectSpace->hpose_base[i].transformMat.m, scaleMat.m);
		a3real4x4Product(mvp_joint->m, mvp_obj.m, scaleMat.m);

		// bone transform
		p = activeHS->hierarchy->nodes[i].parentIndex;
		if (p >= 0)
		{
			// position is parent joint's position
			scaleMat.v3 = activeHS->objectSpace->hpose_base[p].transformMat.v3;

			// direction basis is from parent to current
			a3real3Diff(scaleMat.v2.v,
				activeHS->objectSpace->hpose_base[i].transformMat.v3.v, scaleMat.v3.v);

			// right basis is cross of some upward vector and direction
			// select 'z' for up if either of the other dimensions is set
			a3real3MulS(a3real3CrossUnit(scaleMat.v0.v,
				a3real2LengthSquared(scaleMat.v2.v) > a3real_zero
				? a3vec3_z.v : a3vec3_y.v, scaleMat.v2.v), a3real_sixth);

			// up basis is cross of direction and right
			a3real3MulS(a3real3CrossUnit(scaleMat.v1.v,
				scaleMat.v2.v, scaleMat.v0.v), a3real_sixth);
		}
		else
		{
			// if we are a root joint, make bone invisible
			a3real4x4SetScale(scaleMat.m, a3real_zero);
		}
		a3real4x4Product(mvp_bone->m, mvp_obj.m, scaleMat.m);

		// get base to current object-space
		*t_skin = activeHS->objectSpaceBindToCurrent->hpose_base[i].transformMat;

		// calculate DQ
		a3demo_mat2dquat_safe(dq_skin->Q, t_skin->m);
	}
}


//-----------------------------------------------------------------------------
