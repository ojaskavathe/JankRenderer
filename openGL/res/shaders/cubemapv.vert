#version 460 core
layout (location = 0) in vec3 aPos;

out vec3 texCoords;

uniform mat4 vp;

void main()
{
	//texCoords = vec3(-aPos.x, aPos.yz);
	texCoords = aPos;
	gl_Position = (vp * vec4(aPos, 1.0f)).xyww; 
}
