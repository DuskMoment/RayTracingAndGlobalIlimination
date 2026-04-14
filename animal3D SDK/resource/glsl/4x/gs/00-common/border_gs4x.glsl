#version 450 core
layout (points) in;
layout (line_strip, max_vertices = 8) out;

uniform mat4 uAtlas;
uniform sampler2D uImage00;

out vec4 vTexcoord_atlas;
out vec4 vColor;
  
void main() {   
    
    //top
    vColor = vec4(0.0, -1.0, 0.0, 1.0);
    gl_Position = gl_in[0].gl_Position + vec4(-1.0, 1.0, 0.0, 0.0); 
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4( 1.0, 1.0, 0.0, 0.0);
    EmitVertex();
    
    EndPrimitive();

    //bottom
    vColor = vec4(0.0, 1.0, 0.0, 1.0);
    gl_Position = gl_in[0].gl_Position + vec4(-1.0, -1.0, 0.0, 0.0); 
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4( 1.0, -1.0, 0.0, 0.0);
    EmitVertex();
    
    EndPrimitive();

    //right
    vColor = vec4(-1.0, 0.0, 0.0, 1.0);
    gl_Position = gl_in[0].gl_Position + vec4(1.0, -1.0, 0.0, 0.0); 
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4(1.0, 1.0, 0.0, 0.0);
    EmitVertex();
    
    EndPrimitive();

    //left
    vColor = vec4(1.0, 0.0, 0.0, 1.0);
    gl_Position = gl_in[0].gl_Position + vec4(-1.0, -1.0, 0.0, 0.0); 
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4(-1.0, 1.0, 0.0, 0.0);
    EmitVertex();
    
    EndPrimitive();
}    