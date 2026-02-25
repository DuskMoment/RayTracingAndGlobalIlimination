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
in vec4 vSphere0_orgin;

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

struct HitData
{
	vec4 hitNrl;
	vec4 hitPosition;
};

#define IDX_MODEL_SPHERE0 2
#define IDX_MODEL_SPHERE1 3
#define IDX_ROOM_ENCLOSURE 5
#define IDX_BOX1 0
#define IDX_BOX2 1
#define IDX_LIGHT 4


const float radius_sphere0 = 2.0;
const float radius_sphere1 = 1.0;
const float room_size = 16.0;

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

vec4 CreateRayDirection(vec4 target, vec4 start) // take two positions and return a unit vector 
{
	return normalize(target - start);
}

vec3 CreateRandomRayDirectionOnHem(int seed, vec4 hitNormal)//this will get a new randon direction on a unit sphere 
{

	//get a random vector
	float x = rand(gl_FragCoord.xy + seed) * 2 - 1.0;
	float y = rand(gl_FragCoord.xz + seed) * 2 - 1.0;
	float z = rand(gl_FragCoord.yz + seed) * 2 - 1.0;


//	float x = gold_noise(gl_FragCoord.xy, seed);
//	float y = gold_noise(gl_FragCoord.xz, seed + 1);
//	float z = gold_noise(gl_FragCoord.yz, seed + 2);

	vec3 randVec = vec3(x,y,z);

	//normalize it so it is a unit vector
	randVec = normalize(randVec);

	//dot against the normalize
	vec4 normal =  normalize(hitNormal);
	vec3 testNormal = vec3(normal.x, normal.y, normal.z);

	if(dot(randVec, testNormal) > 0.0)
	{
		return randVec;
	}

	return -randVec;
}

//TODO might need to split this up from a test if hit (bool) and have get color as seperate options)
bool RayCastSphere(const vec4 rayStartPos, vec4 rayDirection, vec4 objPos, float r, out float t, out HitData data)
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
	vec4 rayDir = rayDirection;

	//SPHERE TEST
	vec4 s = objPos - rayStartPos;
	float b = dot(rayDir, s);
	float c = dot(s, s) - r * r;

	float d = b * b - c;

	if(d < 0.0)
	{
		return false;
	}


	d = b - sqrt(d);

	t = d; //used for the out value

	vec4 hitPos = rayStartPos + d * rayDir; //get a position on the thing to hit
	posView = vec4(hitPos.xyz, 1.0);
	posBias = viewer_stack[0].projectionBiasMat * posView;
	//gl_FragDepth = posBias.z / posBias.w; //somthing is messed up with the depth
	
	data.hitNrl =  normalize(hitPos - objPos); 
	data.hitPosition = hitPos;

	return true;

}

//TODO FIX PLANE
bool RayCastPlane(const vec4 rayStartPos, vec4 rayDirection, vec4 objPos, vec4 size, out float t, out vec4 nrlHit)
{
	vec4 newPos = objPos + size;
	vec4 n = normalize(vec4(objPos.xyz - newPos.xyz, 1.0));
	//vec4 rayDir = newPos - rayStartPos;
	float d = dot(rayDirection, n);

	if (d < 1e-8)
	{
		return false;
	}
	
	
	t = (dot(n, newPos) - dot(n, rayStartPos)) / d;
	//i feel like this should be here cuz the ray is pointing towards all visable planes, but only 3/5 visable planes hit with condition
	if (t < 0.0)
	{
		return false;
	}

	vec4 hitPos = rayStartPos + t * rayDirection;
	nrlHit = normalize(n); //if you need the hit normal
	//rtFragColor.rgb = nrlHit * 0.5 + 0.5;
	//rtFragColor.rgb = rayDirection * 0.5 + 0.5;
//
//	vec3 u = normalize(hitPos - newPos);
//	vec3 v = cross(u, normalize(vTangentBasis_view[2]).xyz);
//	vec3 w = normalize(vTangentBasis_view[2]).xyz / dot(normalize(vTangentBasis_view[2]).xyz, normalize(vTangentBasis_view[2]).xyz);
//	float x = dot(w, cross(u, v));
//
//	if (x <= 0 || x >= 1)
//	{
//		return false;
//	}

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

	const vec4 rayPos = vec4(0.0,0.0,0.0,1.0); 

	//bootstrap case
	vec4 objPos = vec4(0.0); //set the first object to be tested
	vec4 rayDirection =  CreateRayDirection(vTangentBasis_view[3], rayPos); //make a ray that will always hit
	float maxT;
	
	float t;
	vec3 color = vec3(0.0);
	vec3 blueColor = vec3(0.0);
	vec3 orangeColor = vec3(0.0);
	vec4 cameraDirection = CreateRayDirection(vTangentBasis_view[3], rayPos);

	const int samples = 10;
	const int rayBounces = 6;
//	int hitIndex[rayBounces];
//	vec4 hitNormal[rayBounces];
//	vec4 lastHitPosition;
//	
	
	int rayCollision =  0;
	int allRayCollisions;

	for(int l = 0; l < samples; l++)
	{
		rayCollision = 0;
		int hitIndex[rayBounces] = {-2,-2,-2,-2,-2,-2};
		vec4 hitNormal[rayBounces];
		vec4 lastHitPosition;
		//reset the camera positions
		vec4 tempRayDirection = CreateRayDirection(vTangentBasis_view[3], rayPos);
		vec4 tempRayPos = rayPos;

		for (int i = 0; i < rayBounces; i++)
		{	
			float minT = 100000.0;
		
			rayHit = false;
			vec4 hitnrl;
			HitData hit;
	

			objPos = model_stack[IDX_MODEL_SPHERE0].modelViewMat[3]; 
			if(RayCastSphere(tempRayPos, tempRayDirection, objPos, radius_sphere0, t, hit))
			{
				if(t < minT && t > -1)
				{
				
					minT = t;
					hitIndex[i] = IDX_MODEL_SPHERE0;
					rayHit = true;
					hitNormal[i] = hit.hitNrl; //an issue with this might be that the hit normal will jsut need to be the first object
					lastHitPosition = hit.hitPosition;

				}
			}

			objPos = model_stack[IDX_MODEL_SPHERE1].modelViewMat[3];
			if(RayCastSphere(tempRayPos, tempRayDirection, objPos, radius_sphere1, t, hit))
			{
				if(t < minT && t > -1)
				{
					//rayDirection = CreateRandomRayDirectionOnHem(i);
					minT = t;
					hitIndex[i] = IDX_MODEL_SPHERE1;
					rayHit = true;
					hitNormal[i] = hit.hitNrl;
					lastHitPosition = hit.hitPosition;
			
				}
			}


			if(rayHit)
			{
			//calulate new rayDirection
				tempRayPos = lastHitPosition;
				tempRayDirection.xyz = CreateRandomRayDirectionOnHem(i, hitNormal[i]);
				rayCollision++;
			}

			if(!rayHit)
			{
				hitIndex[i] = -1;
				break;
			}

		
		}	

		vec3 cellColor;
		for(int j = rayCollision - 1; j >= 0; j--)
		{
		//REDO LIGHING CALCULATIONS
		
			
				if(hitIndex[j] == IDX_MODEL_SPHERE0)
				{
					rayDirection.xyz = CreateRandomRayDirectionOnHem(j + l, hitNormal[j]);
					vec3 tColor = vec3(0.2, 0.4, 0.4);
					rayDirection += (-CreateRayDirection(vTangentBasis_view[3], model_stack[IDX_LIGHT].modelViewMat[3]));
					normalize(rayDirection);
					cellColor += tColor * dot(hitNormal[j].xyz, rayDirection.xyz);
					//color = rayDirection.xyz;
				}
				if(hitIndex[j] == IDX_MODEL_SPHERE1)
				{
					rayDirection.xyz = CreateRandomRayDirectionOnHem(j + l,hitNormal[j]);
					vec3 tColor = vec3(0.6, 0.2, 0.1);
					rayDirection += (-CreateRayDirection(vTangentBasis_view[3], model_stack[IDX_LIGHT].modelViewMat[3]));
					normalize(rayDirection);
					cellColor += tColor * dot(hitNormal[j].xyz, rayDirection.xyz);
					//color += tColor * dot(usednrl.xyz,  -CreateRayDirection(vTangentBasis_view[3], model_stack[IDX_LIGHT].modelViewMat[3]).xyz);
					//color = usednrl.xyz;
				}

				if(hitIndex[j] == -2)
				{
					//we did not run this layer dont add color
					cellColor += vec3(1.0,0.0,0.0);
				}
					
				if(hitIndex[j] == -1)
				{
					cellColor += vec3(1.0,0.0,0.0);//div by the layer
				}
		}
		
		if(rayCollision != 0)
		{
			cellColor /= float(rayBounces);
		}

		color += cellColor;

	}

	
	
	rayHit = false;
	rtFragColor.rgb = color / float(samples);
	//rtFragColor.rgb = color * 0.5 + 0.5;
	//rtFragColor.rgb = color * dot(normalize(vTangentBasis_view[2]).rgb, -cameraDirection);
	rtFragColor.a = 1.0;
	
	//TODO: do lambersion 
	

}