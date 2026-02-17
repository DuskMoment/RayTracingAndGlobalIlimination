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
	
	drawRT_fs4x.glsl
	Output ray-tracing.
*/

#version 450

in vbVertexData {
	mat4 vTangentBasis_view;
	vec4 vTexcoord_atlas;
};

#define MAX_MODELS 24
#define MAX_VIEWERS 1
struct sModelStack
{
	mat4 modelMat;						// model matrix (object -> world)
	mat4 modelMatInverse;				// model inverse matrix (world -> object)
	mat4 modelMatInverseTranspose;		// model inverse-transpose matrix (object -> world skewed)
	mat4 modelViewMat;					// model-view matrix (object -> viewer)
	mat4 modelViewMatInverse;			// model-view inverse matrix (viewer -> object)
	mat4 modelViewMatInverseTranspose;	// model-view inverse transpose matrix (object -> viewer skewed)
	mat4 modelViewProjectionMat;		// model-view-projection matrix (object -> clip)
	mat4 atlasMat;						// atlas matrix (texture -> cell)
};
struct sViewerStack
{
	mat4 projectionMat;					// projection matrix (viewer -> clip)
	mat4 projectionMatInverse;			// projection inverse matrix (clip -> viewer)
	mat4 projectionBiasMat;				// projection-bias matrix (viewer -> biased clip)
	mat4 projectionBiasMatInverse;		// projection-bias inverse matrix (biased clip -> viewer)
	mat4 viewProjectionMat;				// view-projection matrix (world -> clip)
	mat4 viewProjectionMatInverse;		// view-projection inverse matrix (clip -> world)
	mat4 viewProjectionBiasMat;			// view projection-bias matrix (world -> biased clip)
	mat4 viewProjectionBiasMatInverse;	// view-projection-bias inverse matrix (biased clip -> world)
};
uniform ubTransformStack {
	sViewerStack viewer_stack[MAX_VIEWERS];
	sModelStack model_stack[MAX_MODELS];
};

#define IDX_MODEL_SPHERE0 2
#define IDX_MODEL_SPHERE1 3

const float radius_sphere0 = 2.0;
const float radius_sphere1 = 1.0;

uniform mat4 uP;
uniform mat4 uPB;

uniform vec4 uColor;

uniform sampler2D uTex_dm;

layout (location = 0) out vec4 rtFragColor;

void main()
{
	// DUMMY OUTPUT: all fragments are OPAQUE GREEN
	//rtFragColor = vec4(0.0, 1.0, 0.0, 1.0);

//	vec4 sample_dm = texture(uTex_dm, vTexcoord_atlas.xy);
//	rtFragColor = sample_dm * uColor;
//	rtFragColor.a = sample_dm.a;





	//TODO: make a ray from the camera in a direction
		//camera origin * look direction
	const vec3 rayPos = vec3(0.0); //TODO: change this to the camera at some point

	//ray target in screen space coridnates
	float depth = gl_FragCoord.z;
	vec4 uAxis = vec4(1.0); //viewer_stack[0].viewProjectionBiasMatInverse; //error --> this might need to be its own uniform?
	vec4 imagePos = vec4(gl_FragCoord.xy * uAxis.zw, depth, 1.0); //uAxsis might be the uniform Axis meaning the orientatino of the camera
	vec4 posBias = viewer_stack[0].projectionBiasMatInverse * imagePos;
	vec4 posView = posBias / posBias.w;//persepective divide
	vec3 rayTarget = posView.xyz;


	//ray direction
	vec3 rayDir = normalize(rayTarget - rayPos);

	
	//SPHERE TEST
	vec3 objPos = model_stack[IDX_MODEL_SPHERE0].modelViewMat[3].xyz; //error
	vec3 s = objPos - rayPos;
	float b = dot(rayDir, s);
	float c = dot(s, s) - radius_sphere0 * radius_sphere0;

	float d = b * b - c;

	if(d < 0.0)
	{
		gl_FragDepth = depth;
		return;
	}

	d = b - sqrt(d);

	vec3 hitPos = rayPos + d * rayDir; //scale hit point in a direction
	posView = vec4(hitPos, 1.0);
	posBias = viewer_stack[0].projectionBiasMat * posView;
	gl_FragDepth = posBias.z / posBias.w; //also error

	vec3 nrlHit = normalize(hitPos - objPos);

	rtFragColor.rgb = nrlHit * 0.5 + 0.5; 
}
