#version 450

in vec4 vTexcoord_atlas;

const float screenRecip = 0.001253133;

uniform sampler2D uImage00; //prev velocity(xyz) & density(a)
uniform sampler2D uImage01; //current velocity(xyz) & density(a)
uniform sampler2D uImage02; //divergence(x) & pressure(y)
uniform sampler2D uImage03; //prev divergence(x) & pressure(y)

layout (location = 0) out vec4 current;

void main()
{
    float div = -0.5 * screenRecip * 
                    (texture(uImage01, vec2(vTexcoord_atlas.x + screenRecip, vTexcoord_atlas.y)).x - 
                    texture(uImage01, vec2(vTexcoord_atlas.x - screenRecip, vTexcoord_atlas.y)).x +
                    texture(uImage02, vec2(vTexcoord_atlas.x + screenRecip, vTexcoord_atlas.y)).y - 
                    texture(uImage02, vec2(vTexcoord_atlas.x - screenRecip, vTexcoord_atlas.y)).y);

    float p = 0.0;

    current.xy = vec2(div, p);
}
