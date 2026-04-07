#version 450

in vec4 vTexcoord_atlas;

const float screenRecip = 0.001253133;

uniform float a; //dt * diff * N * N
uniform float diffuseDenom; // = 1.0 / (a * 4 + 1)
uniform bool velocity;

uniform sampler2D uImage00; //prev velocity(xyz) & density(a)
uniform sampler2D uImage01; //current velocity(xyz) & density(a)
uniform sampler2D uImage02; //divergence(x) & pressure(y)
uniform sampler2D uImage03; //prev divergence(x) & pressure(y)

layout (location = 0) out vec4 current;

void main()
{
    if (velocity)
    {
        float p = texture(uImage02, vTexcoord_atlas.xy).x +
                  texture(uImage03, vec2(vTexcoord_atlas.x - screenRecip, vTexcoord_atlas.y)).y +
                  texture(uImage03, vec2(vTexcoord_atlas.x + screenRecip, vTexcoord_atlas.y)).y +
                  texture(uImage03, vec2(vTexcoord_atlas.x, vTexcoord_atlas.y - screenRecip)).y +
                  texture(uImage03, vec2(vTexcoord_atlas.x, vTexcoord_atlas.y + screenRecip)).y;

        p *= 0.25;

        current.y = p;
    }
    else
    {
        float adjDiff = texture(uImage01, vec2(vTexcoord_atlas.x - screenRecip, vTexcoord_atlas.y)).a +
                        texture(uImage01, vec2(vTexcoord_atlas.x + screenRecip, vTexcoord_atlas.y)).a +
                        texture(uImage01, vec2(vTexcoord_atlas.x, vTexcoord_atlas.y - screenRecip)).a +
                        texture(uImage01, vec2(vTexcoord_atlas.x, vTexcoord_atlas.y + screenRecip)).a;

         float numerator = texture(uImage00, vTexcoord_atlas.xy).a + a * adjDiff;

         current.a = numerator * diffuseDenom;
    }
}
