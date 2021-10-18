#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat4 instanceMatrix;
layout (location = 7) in vec3 color;

uniform mat4 vp;

out vec3 fcolor;

void main()
{
    gl_Position = vp * instanceMatrix * vec4(aPos, 1.0);
    fcolor = color;
}