#version 450

in vec4 vTexcoord_atlas;
uniform float uTimeStep;
uniform float uDt;

uniform sampler2D uImage00; //prev velocity
uniform sampler2D uImage01; //add force

uniform float uToRangeM;
uniform float uToRangeS;
uniform float uToColorM; 
uniform float uToColorA; 

layout (location = 0) out vec4 current;

float PHI = 1.61803398874989484820459;  //Golden Ratio   

//https://stackoverflow.com/questions/4200224/random-noise-functions-for-glsl
float gold_noise(in vec2 xy, in float seed){
       return fract(tan(distance(xy*PHI, xy)*seed)*xy.x);
}

vec4 randomVel()
{
	vec4 prevVel = texture(uImage00, vTexcoord_atlas.xy);
	vec4 rand = vec4(vec3(gold_noise(gl_FragCoord.xy, uDt + 0.1), gold_noise(gl_FragCoord.xy, uDt + 0.2), gold_noise(gl_FragCoord.xy, uDt + 0.3)), 1.0); //store velocity in 0-1 range
	return rand;
}

//not being used rn
vec2 addSq()
{
	vec4 prevVel = texture(uImage00, vTexcoord_atlas.xy) * uToRangeM - uToRangeS;
	vec4 addForce = texture(uImage01, vTexcoord_atlas.xy) * uToRangeM - uToRangeS;
	prevVel += addForce * uTimeStep * 100;
	prevVel = max(vec4(0.0), min(vec4(1.0), prevVel));
	return prevVel.rg  * uToColorM + uToColorA;
}

void main()
{
	vec2 prevVel = texture(uImage00, vTexcoord_atlas.xy).xy * uToRangeM - uToRangeS;
	vec2 addForce = (texture(uImage01, vTexcoord_atlas.xy).xy * uToRangeM - uToRangeS) * max(vec2(0.0), randomVel().xy);
	vec2 vel = prevVel + addForce * uTimeStep;
	vel = vel * uToColorM + uToColorA;
	vel = max(vec2(0.0), min(vec2(1.0), vel));
	current = vec4(vel, 0.0, 1.0);
}
	
