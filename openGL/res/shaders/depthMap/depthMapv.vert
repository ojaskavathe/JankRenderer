#version 460

layout (location = 0) in vec3 aPos;

uniform mat4 lightVP;
uniform mat4 model;

void main()
{
	gl_Position = lightVP * model * vec4(aPos, 1.0f);
}