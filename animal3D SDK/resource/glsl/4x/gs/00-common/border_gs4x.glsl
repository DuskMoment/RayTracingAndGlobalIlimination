#version 450 core
layout (points) in;
layout (line_strip, max_vertices = 8) out;

out GS_OUT
{
    vec4 gTexcoord_atlas;
    vec2 vColor;
} gs_out;
  
void main() {   
    
    //top
    gs_out.vColor = vec2(0.0, -1.0);
    gl_Position = gl_in[0].gl_Position + vec4(-1.0, 1.0, 0.0, 0.0); 
    gs_out.gTexcoord_atlas = vec4(-1.0, 1.0, 0.0, 1.0); 
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4( 1.0, 1.0, 0.0, 0.0);
    gs_out.gTexcoord_atlas = vec4( 1.0, 1.0, 0.0, 1.0);
    EmitVertex();
    
    EndPrimitive();

    //bottom
    gs_out.vColor = vec2(0.0, 1.0);
    gl_Position = gl_in[0].gl_Position + vec4(-1.0, -1.0, 0.0, 0.0); 
    gs_out.gTexcoord_atlas = vec4(-1.0, -1.0, 0.0, 1.0); 
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4( 1.0, -1.0, 0.0, 0.0);
    gs_out.gTexcoord_atlas = vec4( 1.0, -1.0, 0.0, 1.0);
    EmitVertex();
    
    EndPrimitive();

    //right
    gs_out.vColor = vec2(-1.0, 0.0);
    gl_Position = gl_in[0].gl_Position + vec4(1.0, -1.0, 0.0, 0.0); 
    gs_out.gTexcoord_atlas = vec4(1.0, -1.0, 0.0, 1.0); 
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4(1.0, 1.0, 0.0, 0.0);
    gs_out.gTexcoord_atlas = vec4(1.0, 1.0, 0.0, 1.0);
    EmitVertex();
    
    EndPrimitive();

    //left
    gs_out.vColor = vec2(1.0, 0.0);
    gl_Position = gl_in[0].gl_Position + vec4(-1.0, -1.0, 0.0, 0.0);
    gs_out.gTexcoord_atlas = vec4(-1.0, -1.0, 0.0, 1.0);
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4(-1.0, 1.0, 0.0, 0.0);
    gs_out.gTexcoord_atlas = vec4(-1.0, 1.0, 0.0, 1.0);
    EmitVertex();
    
    EndPrimitive();
}    