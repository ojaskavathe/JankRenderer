#version 460 core
out vec4 FragColor;

in vec3 texCoords;

uniform samplerCube box;

void main()
{
	FragColor = texture(box, texCoords);
}