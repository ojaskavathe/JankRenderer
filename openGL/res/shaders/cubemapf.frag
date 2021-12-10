#version 460 core
out vec4 FragColor;

in vec3 texCoords;

uniform samplerCube box;
uniform float lod = 0.f;

void main()
{
	FragColor = textureLod(box, texCoords, lod);
}