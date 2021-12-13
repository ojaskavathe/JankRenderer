#version 460 core
layout (location = 0) in vec3 aPos;

out vec3 localPos;

uniform mat4 vp;

void main()
{
	localPos = aPos;
	gl_Position = vp * vec4(localPos, 1.f); 
}
