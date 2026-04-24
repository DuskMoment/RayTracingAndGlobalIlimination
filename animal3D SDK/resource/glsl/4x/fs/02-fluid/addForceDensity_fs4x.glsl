#version 450

in vec4 vTexcoord_atlas;

const float screenRecip = 0.00125;
uniform float uTimeStep;
uniform float uDt;

uniform sampler2D uImage00; //prev density
uniform sampler2D uImage01; //add density

layout (location = 0) out vec4 current;

float PHI = 1.61803398874989484820459;  //Golden Ratio   

float gold_noise(in vec2 xy, in float seed){
       return fract(tan(distance(xy*PHI, xy)*seed)*xy.x);
}

vec4 randomVel()
{
	vec4 prevVel = texture(uImage00, vTexcoord_atlas.xy);
	vec4 rand = vec4(vec3(gold_noise(gl_FragCoord.xy, uDt + 0.1), gold_noise(gl_FragCoord.xy, uDt + 0.2), gold_noise(gl_FragCoord.xy, uDt + 0.3)), 1.0); //store velocity in 0-1 range
	return rand;
}

void main()
{
	vec4 col = (texture(uImage00, vTexcoord_atlas.xy)  * 2.0 - 1.0);
	col += (texture(uImage01, vTexcoord_atlas.xy)) * max(vec4(0.0), randomVel());
	col = max(vec4(0.0), min(vec4(1.0), col));
	current =  vec4(col.xyz, 1.0);
}
	
