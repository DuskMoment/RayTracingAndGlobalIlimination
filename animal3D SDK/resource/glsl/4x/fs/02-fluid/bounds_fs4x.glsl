#version 450

in GS_OUT
{
	vec4 gTexcoord_atlas;
	vec2 vColor;
} fs_in;

const float screenRecip = 0.00125;

uniform sampler2D uImage00;
uniform vec4 uAxis;
uniform float uNegate;

layout (location = 0) out vec4 rtFragColor;


void drawTest()
{
	//rtFragColor = vec4(1.0);
	//rtFragColor = vec4(fs_in.vColor * 0.5 + 0.5, 0.0, 1.0);
	rtFragColor = vec4(1.0 - texture(uImage00, fs_in.gTexcoord_atlas.xy * 0.5 + 0.5).rg, 0.0, 1.0);
	//rtFragColor = fs_in.gTexcoord_atlas * 0.5 + 0.5;
	//rtFragColor = texture(uImage00, fs_in.gTexcoord_atlas.xy * 0.5 + 0.5);
	//rtFragColor = vec4(uAxis.y == 980.0);
}


void drawBorder()
{
	vec2 col = texture(uImage00, (fs_in.gTexcoord_atlas.xy * 0.5 + 0.5) + fs_in.vColor * uAxis.zw).xy;
	vec2 vel = uNegate * (col * 2.0 - 1.0); //bring to -1 to 1 to negate
	//vec2 vel = 1.0 - col; //bring to -1 to 1 to negate
	vel = vel * 0.5 + 0.5;
	vel = max(vec2(0.0), vel);
	rtFragColor = vec4(vel, 0.0, 1.0); //bring back to 0-1
}

void main()
{
	drawTest();
	drawBorder();
}
