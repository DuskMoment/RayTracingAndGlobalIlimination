#version 450

in vec4 vTexcoord_atlas;

const float screenRecip = 0.001253133;

uniform sampler2D uImage00; //prev velocity

layout (location = 0) out vec4 current;

void main()
{
    float div = -0.5 * screenRecip * 
                    (texture(uImage00, vec2(vTexcoord_atlas.x + screenRecip, vTexcoord_atlas.y)).x - 
                    texture(uImage00, vec2(vTexcoord_atlas.x - screenRecip, vTexcoord_atlas.y)).x +
                    texture(uImage00, vec2(vTexcoord_atlas.x + screenRecip, vTexcoord_atlas.y)).y - 
                    texture(uImage00, vec2(vTexcoord_atlas.x - screenRecip, vTexcoord_atlas.y)).y);

    float p = 0.0;

    current.xy = vec2(div, p);
}
