#version 460 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;
out vec4 lightSpaceFragPos;
out mat3 TBN;

uniform mat4 model;
uniform mat4 mvp;
uniform mat4 normalMatrix;
uniform mat4 lightVP;

void main()
{
	vec3 T = normalize(vec3(model * vec4(aTangent, 0.f)));
	vec3 B = normalize(vec3(model * vec4(aBitangent, 0.f)));
	vec3 N = normalize(vec3(model * vec4(aNormal, 0.f)));
	TBN = (mat3(T, B, N)); 

	gl_Position = mvp * vec4(aPosition, 1.0f);
	
	FragPos = vec3(model * vec4(aPosition, 1.0f));

	TexCoords = aTexCoord;
	Normal = mat3(normalMatrix) * aNormal;
	lightSpaceFragPos = lightVP * vec4(FragPos, 1.0f);
}