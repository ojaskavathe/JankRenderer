#version 460 core
out vec4 FragColor;

in vec2 texCoords;

uniform sampler2D screenTexture;

void main()
{
	FragColor = vec4(texture(screenTexture, texCoords).rgb, 1.0f);
}