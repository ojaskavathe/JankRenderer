#version 460 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 FragPos;
out vec2 TexCoord;
out vec3 Normal;
//out vec4 lightSpaceFragPos;

uniform mat4 model;
uniform mat4 mvp;
uniform mat4 normalMatrix;
//uniform mat4 lightVP;

void main()
{
	gl_Position = mvp * vec4(aPosition, 1.0f);
	
	FragPos = vec3(model * vec4(aPosition, 1.0f));
	TexCoord = aTexCoord;
	Normal = mat3(model) * aNormal;
	//Normal = mat3(normalMatrix) * aNormal;
	//lightSpaceFragPos = lightVP * vec4(FragPosition, 1.0f);
}