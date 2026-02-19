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
	//vec3 vView_pos;
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
const int samples = 10;

uniform mat4 uP;
uniform mat4 uPB;

uniform vec4 uColor;

uniform sampler2D uTex_dm;

bool rayHit = false;

int hitIndexs[3];//at most 3 bounces

layout (location = 0) out vec4 rtFragColor;

//got this function from https://stackoverflow.com/questions/4200224/random-noise-functions-for-glsl
float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

vec3 CreateRayDirection(vec3 target, vec3 start) // take two positions and return a unit vector 
{
	return normalize(target - start);
}

vec3 CreateRandomRayDirectionOnHem(int model)//this will get a new randon direction on a unit sphere 
{

	bool randSuccess = true;
	vec3 randVec;
	
		//get a random vector
		float x = rand(vTexcoord_atlas.xy);
		float y = rand(vTexcoord_atlas.xy);
		float z = rand(vTexcoord_atlas.xy);

		randVec = vec3(x,y,z);

		//normalize it so it is a unit vector
		randVec = normalize(randVec);

		//idk if this is needed, is for checking if random vector is in sphere
		float lensq = dot(randVec, randVec);
		if (1e-160 < lensq && lensq <= 1)
		{
			randVec = randVec / sqrt(lensq);
			randSuccess = false;
		}
	
	//dot against the normal
	vec4 normal = normalize(vTangentBasis_view[2]);
	//vec4 normal =  model_stack[model].modelViewMatInverse * normalize(vTangentBasis_view[2]); //puts normal back into obj space?
	if(dot(randVec, normal.xyz) > 0.0)
	{
		return randVec;
	}

	return -randVec;
}

//TODO might need to split this up from a test if hit (bool) and have get color as seperate options)
bool RayCastSphere(const vec3 rayStartPos, vec3 rayDirection, vec3 objPos, float r)
{
	vec4 color;

	//use geo position
	//vec3 rayTarget = vTangentBasis_view[3].xyz;

	//ray target in screen space coridnates  -->not used but if you can figure out what uAxis is be my guest
	float depth = gl_FragCoord.z;
	vec4 uAxis = vec4(0.0); //viewer_stack[0].viewProjectionBiasMatInverse; //error --> this might need to be its own uniform?
	vec4 imagePos = vec4(gl_FragCoord.xy * uAxis.zw, depth, 1.0); //uAxsis might be the uniform Axis meaning the orientatino of the camera
	vec4 posBias = viewer_stack[0].projectionBiasMatInverse * imagePos;
	vec4 posView = posBias / posBias.w;//persepective divide
//	vec3 rayTarget = posView.xyz;


	//ray direction
	vec3 rayDir = rayDirection;

	//This section is a attempt at not hitting collision that happen beind the surface normal
//	vec4 normal =  normalize(vTangentBasis_view[2]); 
//
//	//not facing the center
//	if(dot(rayDir, normal.xyz) > 0)
//	{
//		//false;
//		return false;
//	}

	//SPHERE TEST
	vec3 s = objPos - rayStartPos;
	float b = dot(rayDir, s);
	float c = dot(s, s) - r * r;

	float d = b * b - c;

	if(d < 0.0)
	{
		gl_FragDepth = depth;
		return false;
	}


	d = b - sqrt(d);
	vec3 hitPos = rayStartPos + d * rayDir; //get a position on the thing to hit
	posView = vec4(hitPos, 1.0);
	posBias = viewer_stack[0].projectionBiasMat * posView;
	gl_FragDepth = posBias.z / posBias.w; //somthing is messed up with the depth

	vec3 nrlHit = normalize(hitPos - objPos); //if you need the hit normal

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
		color = vec3(1.0,0.0,0.0) * .75;
		break;
	case IDX_MODEL_SPHERE1:
		color = vec3(0.0,1.0,0.0) * .75;
	}

	return color;
}


void main()
{

	//TODO: make a ray from the camera in a direction
		//camera origin * look direction

	const vec3 rayPos = vec3(0,0,0); //starting ray.. in this case the center of the scene
									//TODO: change this to the position of the camera or some point light

	//bootstrap case
	vec3 totalCol = vec3(0.0);
	vec3 objPos = model_stack[IDX_MODEL_SPHERE1].modelViewMat[3].xyz; //set the first object to be tested
	vec3 rayDirOne =  CreateRayDirection(vTangentBasis_view[3].xyz, rayPos); //make a ray that will always hit

	
	if (RayCastSphere(rayPos, rayDirOne, objPos, radius_sphere1))
	{
		for (int i = 0; i < samples; i++)
		{
			vec3 rayDirTwo = CreateRandomRayDirectionOnHem(IDX_MODEL_SPHERE1);
			if (RayCastSphere(rayPos, rayDirTwo, objPos, radius_sphere1))
			{
				for (int j = 0; j < samples; j++)
				{
					vec3 rayDirThree = CreateRandomRayDirectionOnHem(IDX_MODEL_SPHERE1);
					if (RayCastSphere(rayPos, rayDirThree, objPos, radius_sphere1))
					{	
						float a = 0.5 * (rayDirThree.y + 1.0);
						totalCol += ((1.0-a)*vec3(1.0, 1.0, 1.0) + a*vec3(0.5, 0.7, 1.0)) * (0.5 * 2);
					}
				}
			}
			else
			{
				float a = 0.5 * (rayDirTwo.y + 1.0);
				totalCol += ((1.0-a)*vec3(1.0, 1.0, 1.0) + a*vec3(0.5, 0.7, 1.0))  * 0.5;
			}
		}
	}
	else
	{
		float a = 0.5 * (rayDirOne.y + 1.0);
		totalCol += ((1.0-a)*vec3(1.0, 1.0, 1.0) + a*vec3(0.5, 0.7, 1.0));
	}


//	for(int i = 0; i < 3; i++) //first pass check for hits
//	{
//		rayHit = false;
//		//hit first sphere
//		if(RayCastSphere(rayPos, rayDirection, objPos, radius_sphere1) && !rayHit) //check if we hit the object
//		{
//			rayHit = true;
//			hitIndexs[i] = IDX_MODEL_SPHERE1; //add it to the hit list
//
//			//change the starting direction by a unit vector on a hemisphere
//			rayDirection = CreateRandomRayDirectionOnHem(IDX_MODEL_SPHERE1);
//		}
//
//		//hit second sphere
//		vec3 nextPos = model_stack[IDX_MODEL_SPHERE0].modelViewMat[3].xyz; 
//		if (RayCastSphere(rayPos, rayDirection, nextPos, radius_sphere0) && !rayHit)
//		{
//			rayHit = true;
//			hitIndexs[i] = IDX_MODEL_SPHERE0;//add it to the hit list for color
//			rayDirection = CreateRandomRayDirectionOnHem(IDX_MODEL_SPHERE0);//calcualte new direction
//		}
//
//		//TODO add a box check here aswell as the last sphere
//
//	}
//
//	vec3 totalColor;
//	for(int i = 2; i > -1; i--) //make second pass for coloring
//	{
//		//this parese the hit list and colors back ward from the last hit object
//		//you can add some influece based off of the bounce depth
//		totalColor += (GetColorOfObject(hitIndexs[i]));
//	}
	
	//vec4 test = model_stack[2].modelViewMatInverse * normalize(vTangentBasis_view[2]);
	rtFragColor.rgb = totalCol; //out put total color
	rtFragColor.a = 1.0;


	//found nothing just use the base texure
//	vec4 sample_dm = texture(uTex_dm, vTexcoord_atlas.xy);
//	rtFragColor = sample_dm * uColor;
//	rtFragColor.a = sample_dm.a;
}
