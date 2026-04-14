#version 450

in vec4 vTexcoord_atlas;
uniform float uTimeStep;
uniform float uDt;

uniform sampler2D uImage00; //prev velocity(xyz) & density(a)

layout (location = 0) out vec4 current;

float PHI = 1.61803398874989484820459;  //Golden Ratio   

float gold_noise(in vec2 xy, in float seed){
       return fract(tan(distance(xy*PHI, xy)*seed)*xy.x);
}

void main()
{
	vec4 col = texture(uImage00, vTexcoord_atlas.xy);
	vec4 rand = vec4(vec3(gold_noise(gl_FragCoord.xy, 1.0)) * 0.5 + 0.5, 1.0);
	col += uTimeStep * rand;
	current = vec4(col.rg, 0.0, 1.0);
}
	
