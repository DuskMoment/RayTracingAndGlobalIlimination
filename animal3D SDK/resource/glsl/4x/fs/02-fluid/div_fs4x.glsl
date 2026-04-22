#version 450

in vec4 vTexcoord_atlas;

const float screenRecip = 0.00125;

uniform sampler2D uImage00; //current velocity

layout (location = 0) out vec4 current;

void main()
{
    float div = -0.5 * screenRecip * 
                    ((texture(uImage00, vec2(vTexcoord_atlas.x + screenRecip, vTexcoord_atlas.y)).x * 2.0 - 1.0) - 
                    (texture(uImage00, vec2(vTexcoord_atlas.x - screenRecip, vTexcoord_atlas.y)).x * 2.0 - 1.0) +
                    (texture(uImage00, vec2(vTexcoord_atlas.x, vTexcoord_atlas.y + screenRecip)).y * 2.0 - 1.0) - 
                    (texture(uImage00, vec2(vTexcoord_atlas.x, vTexcoord_atlas.y - screenRecip)).y * 2.0 - 1.0));

    float p = 0.0;

    current = vec4(div * 0.5 + 0.5, p, 0.0, 1.0);
}
