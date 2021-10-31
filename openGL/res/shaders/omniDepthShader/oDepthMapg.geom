#version 460 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out;

uniform mat4 lightVP[6];
out vec4 FragPos;

void main()
{
    for(int face = 0; face < 6; ++face)
    {
        gl_Layer = face; // specify which face to render.
        for(int i = 0; i < 3; i++) // for each triangle vertex
        {
            FragPos = gl_in[i].gl_Position;
            gl_Position = lightVP[face] * FragPos;
            EmitVertex();
        }    
        EndPrimitive();
    }
}