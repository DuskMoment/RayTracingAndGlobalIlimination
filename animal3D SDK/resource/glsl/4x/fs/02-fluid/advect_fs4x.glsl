#version 450

in vec4 vTexcoord_atlas;

const float screenRecip = 0.001253133;

uniform float dt;
uniform float N;

uniform sampler2D uImage00; //prev velocity(xyz) & density(a)
uniform sampler2D uImage01; //current velocity(xyz) & density(a)
uniform sampler2D uImage02; //divergence(x) & pressure(y)
uniform sampler2D uImage03; //prev divergence(x) & pressure(y)

uniform bool velocity;

layout (location = 0) out vec4 current;

void main ()
{
	int i0, j0, i1, j1;
    float x, y, s0, t0, s1, t1, dt0;

    dt0 = dt * N;

    //index before timestep
    if (velocity)
    {
         //prev for velocity
        x = gl_FragCoord.x - dt0 * texture(uImage00, vTexcoord_atlas.xy).x; //horizontal
        y = gl_FragCoord.y - dt0 * texture(uImage00, vTexcoord_atlas.xy).y; //vertical
    }
    else
    {
        //current for density
        x = gl_FragCoord.x - dt0 * texture(uImage01, vTexcoord_atlas.xy).x; //horizontal
        y = gl_FragCoord.y - dt0 * texture(uImage01, vTexcoord_atlas.xy).y; //vertical
    }   

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

    if (velocity)
    {
        //for velocity
        vec2 vel = s0 * (t0 * texture(uImage00, vec2(i0, j0) * screenRecip).xy + t1 * texture(uImage00, vec2(i0, j1) * screenRecip).xy) +
                   s1 * (t0 * texture(uImage00, vec2(i1, j0) * screenRecip).xy + t1 * texture(uImage00, vec2(i1, j1) * screenRecip).xy);

        current.rg = vel;
    }
    else
    {
        //neighbor interpolation for final density
        float den = s0 * (t0 * texture(uImage00, vec2(i0, j0) * screenRecip).a + t1 * texture(uImage00, vec2(i0, j1) * screenRecip).a) +
                    s1 * (t0 * texture(uImage00, vec2(i1, j0) * screenRecip).a + t1 * texture(uImage00, vec2(i1, j1) * screenRecip).a);

        current.a = den;
    }
}