#version 450 core
layout (points) in;

out GS_OUT
{
    vec4 gTexcoord_atlas;
    vec2 vColor;
} gs_out;

uniform vec4 uAxis;
uniform float uSize = 2;


layout (triangle_strip, max_vertices = 5) out;
void drawTest()
{
    // Position: edge minus half pixel, converted to NDC
    // Texcoord: edge minus half pixel, converted to UV
    vec2 bl = vec2(0.0, 0.0);
    vec2 br = vec2(uAxis.x, 0.0);
    vec2 tl = vec2(0.0, uAxis.y);
    vec2 tr = vec2(uAxis.x, uAxis.y);

    gs_out.vColor = vec2(0.0, +1.0);// * 0.4 * uAxis.xy;

    gs_out.gTexcoord_atlas.xy = vec2(0.0);
    gl_Position.xy = vec2(-1.0);
    EmitVertex();
    
    gs_out.vColor = vec2(-1.0, 0.0);// * 0.4 * uAxis.xy;
    
    gs_out.gTexcoord_atlas.xy = vec2(1.0, 0.0);
    gl_Position.xy = vec2(1.0, -1.0);
    EmitVertex();

    gs_out.vColor = vec2(0.0, -1.0);// * 0.4 * uAxis.xy;
    
    gs_out.gTexcoord_atlas.xy = vec2(1.0);
    gl_Position.xy = vec2(1.0);
    EmitVertex();
    
    gs_out.vColor = vec2(+1.0, 0.0);// * 0.4 * uAxis.xy;
    
    gs_out.gTexcoord_atlas.xy = vec2(0.0, 1.0);
    gl_Position.xy = vec2(-1.0, 1.0);
    EmitVertex();
    
    gs_out.gTexcoord_atlas.xy = vec2(0.0);
    gl_Position.xy = vec2(-1.0);
    EmitVertex();

    EndPrimitive();
}


//layout (line_strip, max_vertices = 20) out;
void drawBorder() 
{

 //top
 gs_out.vColor = vec2(0.0, -1.0);
 gl_Position = vec4(-1.0, 1.0, -1.0, 1.0); 
 gs_out.gTexcoord_atlas = vec4(-1.0, 1.0, 0.0, 1.0); 
 EmitVertex();

 gl_Position = vec4(-1.0 + uAxis.z * uSize, 1.0 - uAxis.w * uSize, -1.0, 1.0); 
 gs_out.gTexcoord_atlas = vec4(-1.0 + uAxis.z * uSize, 1.0 - uAxis.w * uSize, 0.0, 1.0); 
 EmitVertex();

 gl_Position = vec4(1.0 - uAxis.z * uSize, 1.0 - uAxis.w * uSize, -1.0, 1.0); 
 gs_out.gTexcoord_atlas = vec4(1.0 - uAxis.z * uSize, 1.0 - uAxis.w * uSize, 0.0, 1.0); 
 EmitVertex();

 gl_Position = vec4(1.0, 1.0, -1.0, 1.0);
 gs_out.gTexcoord_atlas = vec4( 1.0, 1.0, 0.0, 1.0);
 EmitVertex();

 gl_Position = vec4(-1.0, 1.0, -1.0, 1.0); 
 gs_out.gTexcoord_atlas = vec4(-1.0, 1.0, 0.0, 1.0); 
 EmitVertex();
 
 EndPrimitive();

 //bottom
 gs_out.vColor = vec2(0.0, 1.0);
 gl_Position = vec4(-1.0 + uAxis.z * uSize, -1.0 + uAxis.w  * uSize, -1.0, 1.0); 
 gs_out.gTexcoord_atlas = vec4(-1.0 + uAxis.z  * uSize, -1.0 + uAxis.w  * uSize, 0.0, 1.0); 
 EmitVertex();

 gs_out.vColor = vec2(0.0, 1.0);
 gl_Position = vec4(-1.0, -1.0, -1.0, 1.0); 
 gs_out.gTexcoord_atlas = vec4(-1.0, -1.0, 0.0, 1.0); 
 EmitVertex();

 gl_Position = vec4( 1.0, -1.0, -1.0, 1.0);
 gs_out.gTexcoord_atlas = vec4( 1.0, -1.0, 0.0, 1.0);
 EmitVertex();

 gl_Position = vec4(1.0 - uAxis.z * uSize, -1.0 + uAxis.w * uSize, -1.0, 1.0); 
 gs_out.gTexcoord_atlas = vec4(1.0 - uAxis.z * uSize, -1.0 + uAxis.w * uSize, 0.0, 1.0); 
 EmitVertex();

  gl_Position = vec4(-1.0 + uAxis.z * uSize, -1.0 + uAxis.w * uSize, -1.0, 1.0); 
 gs_out.gTexcoord_atlas = vec4(-1.0 + uAxis.z * uSize, -1.0 + uAxis.w * uSize, 0.0, 1.0); 
 EmitVertex();
 
 EndPrimitive();

 //right
 gs_out.vColor = vec2(-1.0, 0.0);
 gl_Position = vec4(1.0 - uAxis.z * uSize, 1.0 - uAxis.w * uSize, -1.0, 1.0); 
 gs_out.gTexcoord_atlas = vec4(1.0 - uAxis.z * uSize, 1.0 - uAxis.w * uSize, 0.0, 1.0); 
 EmitVertex();

 gl_Position = vec4(1.0 - uAxis.z * uSize, -1.0 + uAxis.w * uSize, -1.0, 1.0); 
 gs_out.gTexcoord_atlas = vec4(1.0 - uAxis.z * uSize, -1.0 + uAxis.w * uSize, 0.0, 1.0); 
 EmitVertex();

 gl_Position = vec4(1.0, -1.0, -1.0, 1.0); 
 gs_out.gTexcoord_atlas = vec4(1.0, -1.0, 0.0, 1.0); 
 EmitVertex();

 gl_Position = vec4(1.0, 1.0, -1.0, 1.0);
 gs_out.gTexcoord_atlas = vec4(1.0, 1.0, 0.0, 1.0);
 EmitVertex();

 gl_Position = vec4(1.0 - uAxis.z * uSize, 1.0 - uAxis.w * uSize, -1.0, 1.0); 
 gs_out.gTexcoord_atlas = vec4(1.0 - uAxis.z * uSize, 1.0 - uAxis.w * uSize, 0.0, 1.0); 
 EmitVertex();
 
 EndPrimitive();

 //left
 gs_out.vColor = vec2(1.0, 0.0);
 gl_Position = vec4(-1.0, -1.0, -1.0, 1.0);
 gs_out.gTexcoord_atlas = vec4(-1.0, -1.0, 0.0, 1.0);
 EmitVertex();

  gl_Position = vec4(-1.0 + uAxis.z * uSize, -1.0 + uAxis.w * uSize, -1.0, 1.0); 
 gs_out.gTexcoord_atlas = vec4(-1.0 + uAxis.z * uSize, -1.0 + uAxis.w * uSize, 0.0, 1.0); 
 EmitVertex();

 gl_Position = vec4(-1.0 + uAxis.z * uSize, 1.0 - uAxis.w * uSize, -1.0, 1.0); 
 gs_out.gTexcoord_atlas = vec4(-1.0 + uAxis.z * uSize, 1.0 - uAxis.w * uSize, 0.0, 1.0); 
 EmitVertex();

 gl_Position = vec4(-1.0, 1.0, -1.0, 1.0);
 gs_out.gTexcoord_atlas = vec4(-1.0, 1.0, 0.0, 1.0);
 EmitVertex();

 gl_Position = vec4(-1.0, -1.0, -1.0, 1.0);
 gs_out.gTexcoord_atlas = vec4(-1.0, -1.0, 0.0, 1.0);
 EmitVertex();
 
 EndPrimitive();
}

void main()
{
    //drawBorder();
    drawTest();
}