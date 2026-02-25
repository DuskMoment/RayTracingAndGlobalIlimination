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
bool RayCastSphere(const vec4 rayStartPos, vec4 rayDirection, vec4 objPos, float r, out float t, out vec4 nrlHit)
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

	nrlHit = normalize(hitPos - objPos); 
	//nrlHit = normalize(objPos); 
	
	return true;

}

bool RayCastPlane(const vec4 rayStartPos, vec4 rayDirection, vec4 objPos, vec4 size, out float t, out vec4 nrlHit)
{
	mat3 TBN;
	vec3 newPos = objPos.xyz + normalize(size.xyz) * 8.0; 
	vec3 n = normalize(size).xyz;
	float d = dot(rayDirection.xyz, n);

	if (d < 1e-8)
	{
		return false;
	}
	
	
	t = (dot(n, newPos) - dot(n, rayStartPos.xyz)) / d;
	if (t < 0.0)
	{
		return false;
	}

	vec4 hitPos = rayStartPos + t * rayDirection;
	
	//rtFragColor.rgb = nrlHit.xyz * 0.5 + 0.5;
	//rtFragColor.rgb = rayDirection.xyz * 0.5 + 0.5;

	vec3 w = n / dot(n, n);
	float a = dot(w, cross(hitPos.xyz, TBN[1]));
	float b = dot(w, cross(TBN[2], hitPos.xyz));

	if (a < 0 || a > 1)
	{
		return false;
	}

	if (b < 0 || b > 1)
	{
		return false;
	}

	nrlHit = vec4(normalize(hitPos.xyz - objPos.xyz), 0.0);

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

	int samples = 16;
	

	for (int i = 0; i < samples; i++)
	{	
		float minT = 100000.0;
		int toDraw = -1;
		rayHit = false;
		rayDirection =  CreateRayDirection(vTangentBasis_view[3], rayPos);
		vec4 hitnrl;
		vec4 usednrl;

		objPos = model_stack[IDX_ROOM_ENCLOSURE].modelViewMat[3];

		//enclosure
		//left
		if(RayCastPlane(rayPos, rayDirection, objPos, model_stack[IDX_ROOM_ENCLOSURE].modelViewMat[0], t, hitnrl))
		{
			if(t < minT && t > -1)
			{
				
				minT = t;
				toDraw = IDX_ROOM_ENCLOSURE;
				rayHit = true;
				usednrl = hitnrl;
			}
		}

		//right
		if(RayCastPlane(rayPos, rayDirection, objPos, -model_stack[IDX_ROOM_ENCLOSURE].modelViewMat[0], t, hitnrl))
		{
			if(t < minT && t > -1)
			{
				
				minT = t;
				toDraw = IDX_ROOM_ENCLOSURE;
				rayHit = true;
				usednrl = hitnrl;
			}
		}

		//backward
		if(RayCastPlane(rayPos, rayDirection, objPos, model_stack[IDX_ROOM_ENCLOSURE].modelViewMat[1], t, hitnrl))
		{
			if(t < minT && t > -1)
			{
				
				minT = t;
				toDraw = IDX_ROOM_ENCLOSURE;
				rayHit = true;
				usednrl = hitnrl;
			}
		}
		
		//forward
		if(RayCastPlane(rayPos, rayDirection, objPos, -model_stack[IDX_ROOM_ENCLOSURE].modelViewMat[1], t, hitnrl))
		{
			if(t < minT && t > -1)
			{
				
				minT = t;
				toDraw = IDX_ROOM_ENCLOSURE;
				rayHit = true;
				usednrl = hitnrl;
			}
		}

		//bottom
		if(RayCastPlane(rayPos, rayDirection, objPos, model_stack[IDX_ROOM_ENCLOSURE].modelViewMat[2], t, hitnrl))
		{
			if(t < minT && t > -1)
			{
				
				minT = t;
				toDraw = IDX_ROOM_ENCLOSURE;
				rayHit = true;
				usednrl = hitnrl;
			}
		}

		//top
		if(RayCastPlane(rayPos, rayDirection, objPos, -model_stack[IDX_ROOM_ENCLOSURE].modelViewMat[2], t, hitnrl))
		{
			if(t < minT && t > -1)
			{
				
				minT = t;
				toDraw = IDX_ROOM_ENCLOSURE;
				rayHit = true;
				usednrl = hitnrl;
			}
		}
		
		objPos = model_stack[IDX_MODEL_SPHERE0].modelViewMat[3]; 
		if(RayCastSphere(rayPos, rayDirection, objPos, radius_sphere0, t, hitnrl))
		{
			if(t < minT && t > -1)
			{
				
				minT = t;
				toDraw = IDX_MODEL_SPHERE0;
				rayHit = true;
				usednrl = hitnrl;
			}
		}

		objPos = model_stack[IDX_MODEL_SPHERE1].modelViewMat[3];
		if(RayCastSphere(rayPos, rayDirection, objPos, radius_sphere1, t, hitnrl))
		{
			if(t < minT && t > -1)
			{
				//rayDirection = CreateRandomRayDirectionOnHem(i);
				minT = t;
				toDraw = IDX_MODEL_SPHERE1;
				rayHit = true;
				usednrl = hitnrl;
			
			}
		}
		

		//REDO LIGHING CALCULATIONS
		if(rayHit)
		{
			if(toDraw == IDX_MODEL_SPHERE0)
			{
				rayDirection.xyz = CreateRandomRayDirectionOnHem(i, usednrl);
				vec3 tColor = vec3(0.2, 0.4, 0.4);
				rayDirection += usednrl;
				rayDirection += (-CreateRayDirection(vTangentBasis_view[3], model_stack[IDX_LIGHT].modelViewMat[3]));
				normalize(rayDirection);
				color += tColor * dot(usednrl.xyz, rayDirection.xyz);
				//color = rayDirection.xyz;
				//color += usednrl.xyz * 0.5 + 0.5;
			}
			if(toDraw == IDX_MODEL_SPHERE1)
			{
				rayDirection.xyz = CreateRandomRayDirectionOnHem(i,usednrl);
				vec3 tColor = vec3(0.6, 0.2, 0.1);
				rayDirection += usednrl;
				rayDirection += (-CreateRayDirection(vTangentBasis_view[3], model_stack[IDX_LIGHT].modelViewMat[3]));
				normalize(rayDirection);
				color += tColor * dot(usednrl.xyz, rayDirection.xyz);
				//color += tColor * dot(usednrl.xyz,  -CreateRayDirection(vTangentBasis_view[3], model_stack[IDX_LIGHT].modelViewMat[3]).xyz);
				//color += usednrl.xyz * 0.5 + 0.5;
			}
			if(toDraw == IDX_ROOM_ENCLOSURE)
			{
				rayDirection.xyz = CreateRandomRayDirectionOnHem(i, usednrl);
				vec3 tColor = vec3(0.6, 0.2, 0.65);
				rayDirection += usednrl;
				rayDirection += (-CreateRayDirection(vTangentBasis_view[3], model_stack[IDX_LIGHT].modelViewMat[3]));
				normalize(rayDirection);
				color += tColor * dot(usednrl.xyz, rayDirection.xyz);
				//color +=  tColor * dot(usednrl.xyz,- CreateRayDirection(vTangentBasis_view[3], rayPos).xyz);
				//color += usednrl.xyz * 0.5 + 0.5;
				//color = model_stack[IDX_ROOM_ENCLOSURE].modelViewMat[2].xyz;
			}
		}

			
		
		if(!rayHit)
		{
			color += vec3(1.0,1.0,0.0);
		}
	}
	
	rayHit = false;
	rtFragColor.rgb = color / float(samples);
	//rtFragColor.rgb = color * 0.5 + 0.5;
	//rtFragColor.rgb = color * dot(normalize(vTangentBasis_view[2]).rgb, -cameraDirection);
	rtFragColor.a = 1.0;
	
	//TODO: do lambersion 
	

}