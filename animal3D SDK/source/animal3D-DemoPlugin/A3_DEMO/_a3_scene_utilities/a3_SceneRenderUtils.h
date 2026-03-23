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
	
	a3_SceneRenderUtils.h
	Drawing utilities for scene state.
*/

#ifndef __ANIMAL3D_SCENERENDERUTILS_H
#define __ANIMAL3D_SCENERENDERUTILS_H


//-----------------------------------------------------------------------------
// animal3D framework includes

#include "animal3D/animal3D.h"
#include "animal3D-A3DG/animal3D-A3DG.h"
#include "animal3D-A3DM/animal3D-A3DM.h"


//-----------------------------------------------------------------------------
// scene includes

#include "a3_SceneShaderProgram.h"


//-----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C"
{
#else	// !__cplusplus
#endif	// __cplusplus

	
//-----------------------------------------------------------------------------
// general utilities

// get the inverse transpose of a matrix
inline a3real4x4r a3scene_quickInverseTranspose_internal(a3real4x4p m_out, const a3real4x4p m_in);

// invert and transpose a matrix
inline a3real4x4r a3scene_quickInvertTranspose_internal(a3real4x4p m_inout);

// get the transpose of a matrix with the bottom row set to zero
inline a3real4x4r a3scene_quickTransposedZeroBottomRow(a3real4x4p m_out, const a3real4x4p m_in);


//-----------------------------------------------------------------------------
// rendering states

// set default blending mode for composition
inline void a3scene_enableCompositeBlending();

// set default blending mode for accumulation
inline void a3scene_enableAdditiveBlending();

// set default state for writing to stencil buffer
inline void a3scene_enableStencilWrite();

// set default state for comparing with stencil buffer
inline void a3scene_enableStencilCompare();

// set default state for renderer
inline void a3scene_setDefaultGraphicsState();

// set state for scene draw
inline void a3scene_setSceneState(a3_Framebuffer const* currentWriteFBO, a3boolean displaySkybox);


//-----------------------------------------------------------------------------

inline void a3scene_drawModelSimple(a3real4x4p modelViewProjectionMat, a3real4x4p const viewProjectionMat, a3real4x4p const modelMat, a3_SceneShaderProgram const* program);

inline void a3scene_drawModelSimple_activateModel(a3real4x4p modelViewProjectionMat, a3real4x4p const viewProjectionMat, a3real4x4p const modelMat, a3_SceneShaderProgram const* program, a3_VertexDrawable const* drawable);

inline void a3scene_drawModelSolidColor(a3real4x4p modelViewProjectionMat, a3real4x4p const viewProjectionMat, a3real4x4p const modelMat, a3_SceneShaderProgram const* program, a3_VertexDrawable const* drawable, a3real4p const color);

inline void a3scene_drawModelTexturedColored_invertModel(a3real4x4p modelViewProjectionMat, a3real4x4p const viewProjectionMat, a3real4x4p const modelMat, a3real4x4p const atlasMat, a3_SceneShaderProgram const* program, a3_VertexDrawable const* drawable, a3_Texture const* texture, a3real4p const color);

inline void a3scene_drawModelLighting(a3real4x4p modelViewProjectionMat, a3real4x4p modelViewMat, a3real4x4p const viewProjectionMat, a3real4x4p const viewMat, a3real4x4p const modelMat, a3_SceneShaderProgram const* program, a3_VertexDrawable const* drawable, a3real4p const color);

inline void a3scene_drawModelLighting_bias_other(a3real4x4p modelViewProjectionBiasMat_other, a3real4x4p modelViewProjectionMat, a3real4x4p modelViewMat, a3real4x4p const viewProjectionBiasMat_other, a3real4x4p const viewProjectionMat, a3real4x4p const viewMat, a3real4x4p const modelMat, a3_SceneShaderProgram const* program, a3_VertexDrawable const* drawable, a3real4p const color);

inline void a3scene_drawStencilTest(a3real4x4p modelViewProjectionMat, a3real4x4p const viewProjectionMat, a3real4x4p const modelMat, a3_SceneShaderProgram const* program, a3_VertexDrawable const* drawable);


//-----------------------------------------------------------------------------

//inline a3i32 a3SSBOCreate(a3i32 uniformLocation);
//
//inline a3ret a3SSBOSet(a3i32 uniformLocation, a3i64 bytes, const void* data);

//-----------------------------------------------------------------------------


#ifdef __cplusplus
}
#endif	// __cplusplus


#include "_inl/a3_SceneRenderUtils.inl"


#endif	// !__ANIMAL3D_SCENERENDERUTILS_H