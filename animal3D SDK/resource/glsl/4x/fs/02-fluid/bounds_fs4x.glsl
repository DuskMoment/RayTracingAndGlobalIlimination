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
uniform float uToRangeM;
uniform float uToRangeS;
uniform float uToColorM; 
uniform float uToColorA; 

layout (location = 0) out vec4 rtFragColor;


void drawTest()
{
	//rtFragColor = vec4(1.0, 0.0, 0.0, 1.0);
	//rtFragColor = vec4(fs_in.vColor * 0.5 + 0.5, 0.0, 1.0);
	//rtFragColor = vec4(1.0 - texture(uImage00, fs_in.gTexcoord_atlas.xy * 0.5 + 0.5).rg, 0.0, 1.0);
	rtFragColor = fs_in.gTexcoord_atlas;
	//rtFragColor = texture(uImage00, fs_in.gTexcoord_atlas.xy * 0.5 + 0.5);
	//rtFragColor = vec4(uAxis.y == 980.0);
}

void drawBad()
{
	vec2 col = texture(uImage00, (fs_in.gTexcoord_atlas.xy) + fs_in.vColor * uAxis.zw).xy * uToRangeM - uToRangeS;
	if (fs_in.gTexcoord_atlas.x >= uAxis.x * uAxis.z - uAxis.z)
	{
		col *= uNegate;
	}

	if (fs_in.gTexcoord_atlas.x <= uAxis.z)
	{
		col *= uNegate;
	}

	if (fs_in.gTexcoord_atlas.y >= uAxis.y * uAxis.w - uAxis.w)
	{
		col *= uNegate;
	}

	if (fs_in.gTexcoord_atlas.y <= uAxis.w)
	{
		col *= uNegate;
	}

	rtFragColor = vec4(col * uToColorM + uToColorA, 0.0, 1.0);
}


void drawBorder()
{
	vec2 col = texture(uImage00, (fs_in.gTexcoord_atlas.xy * 0.5 + 0.5) + fs_in.vColor * uAxis.zw).xy;
	vec2 vel = uNegate * (col * uToRangeM - uToRangeS); //bring to -1 to 1 to negate
	//vec2 vel = 1.0 - col; //bring to -1 to 1 to negate
	vel = vel * uToColorM + uToColorA;
	vel = max(vec2(0.0), vel);
	rtFragColor = vec4(vel, 0.0, 1.0); //bring back to 0-1
}

void main()
{
	//drawTest();
	//drawBorder();
	drawBad();
}
