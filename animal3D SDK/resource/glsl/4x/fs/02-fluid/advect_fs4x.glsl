#version 450

in vec4 vTexcoord_atlas;

uniform float uScreenRecip;

uniform float dt;
uniform float N;

uniform sampler2D uImage00; //if velocity step prev velocity, if density step prev density
uniform sampler2D uImage01; //current velocity

uniform float uToRangeM;
uniform float uToRangeS;
uniform float uToColorM; 
uniform float uToColorA; 

uniform bool velocity;

layout (location = 0) out vec4 current;

void main ()
{
	int i0, j0, i1, j1;
    float x, y, s0, t0, s1, t1, dt0;

    dt0 = dt * N;

     x = gl_FragCoord.x - dt0 * (texture(uImage01, vTexcoord_atlas.xy).x * uToRangeM - uToRangeS); //horizontal
     y = gl_FragCoord.y - dt0 * (texture(uImage01, vTexcoord_atlas.xy).y * uToRangeM - uToRangeS); //vertical

    //clamps edge cases
    x = clamp(x, 0.5, float(N) + 0.5);

    //current i and one over i
    i0 = int(x); 
    i1 = i0 + 1;

    //clamps edge case
    y = clamp(y, 0.5, float(N) + 0.5);
    

    //current and down 1 j
    j0 = int(y);
    j1 = j0 + 1;

    //interpolation weights
    s1 = x - i0; 
    s0 = 1 - s1;
    t1 = y - j0; 
    t0 = 1 - t1;

     vec3 col = s0 * (t0 * (texture(uImage00, vec2(i0, j0) * uScreenRecip).xyz * uToRangeM - uToRangeS) + t1 * (texture(uImage00, vec2(i0, j1) * uScreenRecip).xyz * uToRangeM - uToRangeS)) +
                s1 * (t0 * (texture(uImage00, vec2(i1, j0) * uScreenRecip).xyz * uToRangeM - uToRangeS) + t1 * (texture(uImage00, vec2(i1, j1) * uScreenRecip).xyz * uToRangeM - uToRangeS));

    current = vec4(col * uToColorM + uToColorA, 1.0);
}