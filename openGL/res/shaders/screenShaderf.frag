#version 460 core
out vec4 FragColor;

in vec2 texCoords;

uniform sampler2D screenTexture;
uniform float gamma;

void main()
{
	vec3 res = texture(screenTexture, texCoords).rgb; 
	vec3 gammaCorrected = pow(res, vec3(1/gamma));
	FragColor = vec4(gammaCorrected, 1.0f);
}