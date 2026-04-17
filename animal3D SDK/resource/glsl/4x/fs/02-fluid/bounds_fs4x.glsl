#version 450

in GS_OUT
{
	vec4 gTexcoord_atlas;
	vec2 vColor;
} fs_in;

const float screenRecip = 0.00125;

uniform sampler2D uImage00;
uniform float uNegate;

layout (location = 0) out vec4 rtFragColor;

void main()
{
	//rtFragColor = vec4(1.0, 0.0, 0.0, 1.0);
	//rtFragColor = vec4(fs_in.gTexcoord_atlas.xy * 0.5 + 0.5 + fs_in.vColor * screenRecip, 0.0, 1.0);

	vec2 col = texture(uImage00, (fs_in.gTexcoord_atlas.xy * 0.5 + 0.5) + fs_in.vColor * screenRecip).xy;
	//col = vec2(1.0, 1.0);
	vec2 vel = -1.0 * (col * 2.0 - 1.0); //bring to -1 to 1 to negate
	vel = vel * 0.5 + 0.5;
	vel = max(vec2(0.0), vel);
	rtFragColor = vec4(vel, 0.0, 1.0); //bring back to 0-1
	//rtFragColor = vec4(col, 1.0); //bring back to 0-1
	//rtFragColor = vec4(0.0, 0.0, 0.0, 1.0); 
}
