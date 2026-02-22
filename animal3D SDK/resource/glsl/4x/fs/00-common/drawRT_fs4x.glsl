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

in vec4 vPos;

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

uniform vec3 pos;
uniform mat4 uP;
uniform mat4 uPB;
uniform float uTime;

uniform vec4 uColor;

uniform sampler2D uTex_dm;

bool rayHit = false;

int hitIndexs[3];//at most 3 bounces
float minT[3];

layout (location = 0) out vec4 rtFragColor;

float PHI = 1.61803398874989484820459;  //Golden Ratio   

float gold_noise(in vec2 xy, in float seed){
       return fract(tan(distance(xy*PHI, xy)*seed)*xy.x);
}

//got this function from https://stackoverflow.com/questions/4200224/random-noise-functions-for-glsl
float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

vec3 CreateRayDirection(vec3 target, vec3 start) // take two positions and return a unit vector 
{
	return normalize(target - start);
}

vec3 CreateRandomRayDirectionOnHem(int seed)//this will get a new randon direction on a unit sphere 
{

	//get a random vector
	float x = rand(gl_FragCoord.xy + seed);
	float y = rand(gl_FragCoord.xz + seed);
	float z = rand(gl_FragCoord.yz + seed);

//	float x = gold_noise(gl_FragCoord.xy, seed);
//	float y = gold_noise(gl_FragCoord.xz, seed + 1);
//	float z = gold_noise(gl_FragCoord.yz, seed + 2);

	vec3 randVec = vec3(x,y,z);

	//normalize it so it is a unit vector
	randVec = normalize(randVec);

	//dot against the normalize
	vec4 normal =  normalize(vTangentBasis_view[2]);
	vec3 testNormal = vec3(normal.x, normal.y, normal.z);

	if(dot(randVec, testNormal) > 0.0)
	{
		return randVec;
	}

	return -randVec;
}

//TODO might need to split this up from a test if hit (bool) and have get color as seperate options)
bool RayCastSphere(const vec3 rayStartPos, vec3 rayDirection, vec3 objPos, float r, out float t)
{
	vec4 color;

	//use geo position
	//vec3 rayTarget = vTangentBasis_view[3].xyz;

	//ray target in screen space coridnates  -->not used but if you can figure out what uAxis is be my guest
	float depth = gl_FragCoord.z;
	vec4 uAxis = vec4(0.0); //viewer_stack[0].viewProjectionBiasMatInverse; //error --> this might need to be its own uniform?
	vec4 imagePos = vec4(gl_FragCoord.xy * uAxis.zw, depth, 1.0); //uaxis screen resolition
	vec4 posBias = viewer_stack[0].projectionBiasMatInverse * imagePos;
	vec4 posView = posBias / posBias.w;//persepective divide
//	vec3 rayTarget = posView.xyz;


	//ray direction
	vec3 rayDir = rayDirection;

	//SPHERE TEST
	vec3 s = objPos - rayStartPos;
	float b = dot(rayDir, s);
	float c = dot(s, s) - r * r;

	float d = b * b - c;

	if(d < 0.0)
	{
		return false;
	}


	d = b - sqrt(d);

	t = d; //used for the out value

	vec3 hitPos = rayStartPos + d * rayDir; //get a position on the thing to hit
	posView = vec4(hitPos, 1.0);
	posBias = viewer_stack[0].projectionBiasMat * posView;
	//gl_FragDepth = posBias.z / posBias.w; //somthing is messed up with the depth

	vec3 nrlHit = normalize(hitPos - objPos); //if you need the hit normal
	//rtFragColor.rgb = nrlHit * 0.5 + 0.5;

	return true;

}

//this function gets the color of a object at a model index
vec3 GetColorOfObject(int modelIndex)
{
//get the color based off of the object that we hit
	vec3 color = vec3(0.0, 0.0, 0.0);

	switch(modelIndex)
	{
	case IDX_MODEL_SPHERE0: 
		color = vec3(1.0,0.0,0.0) * 0.7;
		break;
	case IDX_MODEL_SPHERE1:
		color = vec3(0.0,0.0,1.0) * 0.7;
	}

	return color;
}


void main()
{
	const vec3 rayPos = vPos.xyz; 

	//bootstrap case
	vec3 objPos = model_stack[IDX_MODEL_SPHERE1].modelViewMat[3].xyz; //set the first object to be tested
	vec3 rayDirection =  CreateRayDirection(vTangentBasis_view[3].xyz, rayPos); //make a ray that will always hit
	float maxT;
	float minT = 100000f;
	float t;
	vec3 color = vec3(0.0);
	vec3 blueColor = vec3(0.0);
	vec3 orangeColor = vec3(0.0);

	

	for (int i = 0; i < 1000; i++)
	{	
		int toDraw;
		rayHit = false;
		rayDirection =  CreateRayDirection(vTangentBasis_view[3].xyz, rayPos);
		objPos = model_stack[IDX_MODEL_SPHERE0].modelViewMat[3].xyz;
		
		if(RayCastSphere(rayPos, rayDirection, objPos, radius_sphere0, t))
		{
			if(t < minT && t > -1)
			{
				
				minT = t;
				toDraw = IDX_MODEL_SPHERE0;
				
				rayHit = true;
			}
		}

		objPos = model_stack[IDX_MODEL_SPHERE1].modelViewMat[3].xyz;
		if(RayCastSphere(rayPos, rayDirection, objPos, radius_sphere1, t))
		{
			if(t < minT && t > -1)
			{
				rayDirection = CreateRandomRayDirectionOnHem(i);
				minT = t;
				toDraw = IDX_MODEL_SPHERE1;
				
				rayHit = true;
			
			}
		}


		if(rayHit)
		{
			if(toDraw == IDX_MODEL_SPHERE0)
			{
				rayDirection = CreateRandomRayDirectionOnHem(i);
				vec3 tColor = vec3(0.2, 0.4, 0.4);
				color += tColor * dot(normalize(vTangentBasis_view[2]).rgb, rayDirection);
			}
			if(toDraw == IDX_MODEL_SPHERE1)
			{
				rayDirection = CreateRandomRayDirectionOnHem(i);
				vec3 tColor = vec3(0.6, 0.2, 0.1);
				color += tColor * dot(normalize(vTangentBasis_view[2]).rgb, rayDirection);
			}
		
		}
	}



	// seeded random, does different things at different angles, colors add properly, shading on large sphere looks good, colors walls
//	for (int i = 0; i < 1000; i++)
//	{
//		rayDirection = CreateRandomRayDirectionOnHem(i);
//		objPos = model_stack[IDX_MODEL_SPHERE0].modelViewMat[3].xyz;
//		if(RayCastSphere(rayPos, rayDirection, objPos, radius_sphere0, t))
//		{
//			if(t < minT)
//			{
//		
//			minT = t;
//				toDraw = IDX_MODEL_SPHERE0;
//				vec3 tColor = vec3(0.2, 0.4, 0.4);
//				color += tColor * dot(normalize(vTangentBasis_view[2]).rgb, rayDirection);
//				rayHit = true;
//			}
//		}
//
//		objPos = model_stack[IDX_MODEL_SPHERE1].modelViewMat[3].xyz;
//		if(RayCastSphere(rayPos, rayDirection, objPos, radius_sphere1, t))
//		{
//			if(t < minT)
//			{
//				minT = t;
//				toDraw = IDX_MODEL_SPHERE1;
//				vec3 tColor = vec3(0.6, 0.2, 0.1);
//				color += tColor * dot(normalize(vTangentBasis_view[2]).rgb, rayDirection);
//				rayHit = true;
//			
//			}
//		}
//	}
	
	rayHit = false;
	rtFragColor.rgb = color;
	rtFragColor.rgb =  color * dot(normalize(vTangentBasis_view[2]).rgb, -rayDirection); //if ray dir is view pos -> obj
	rtFragColor.a = 1.0;
	
	//TODO: do lambersion 
	

}
