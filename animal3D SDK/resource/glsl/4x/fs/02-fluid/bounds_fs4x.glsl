#version 450

in vec4 vColor;
in vec4 gTexcoord_atlas;

uniform sampler2D uImage00;
uniform float uNegate;

layout (location = 0) out vec4 rtFragColor;

void main()
{
	// DUMMY OUTPUT: all fragments are OPAQUE ORANGE
	//rtFragColor = vec4(1.0, 0.5, 0.0, 1.0);
	rtFragColor = -1.0 * texture(uImage00, (gl_FragCoord.xy + vColor.xy) / 800);
}
