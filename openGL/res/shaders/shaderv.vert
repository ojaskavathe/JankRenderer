#version 460 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec2 texCoord;

//NOTE: flat only works when the model has 3 normals per vertex, or rather 3 separate vertices so it's kinda memory heavy
out vec3 Normal;	//flat indicates no interpolation 
out vec3 FragPosition;

uniform mat4 mvp;
uniform mat4 model;
uniform mat4 normalMatrix;

void main()
{
	gl_Position = mvp * vec4(aPosition, 1.0f);
	
	FragPosition = vec3(model * vec4(aPosition, 1.0f));
	texCoord = aTexCoord;
	Normal = mat3(normalMatrix) * aNormal;
}