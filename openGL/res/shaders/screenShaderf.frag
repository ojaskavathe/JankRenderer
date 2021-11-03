#version 460 core
out vec4 FragColor;

in vec2 texCoords;

uniform sampler2D screenTexture;
uniform float gamma;
uniform float exposure;

void main()
{
	vec3 res = texture(screenTexture, texCoords).rgb;

	vec3 mapped = vec3(1.f) - exp(-res * exposure);

	vec3 gammaCorrected = pow(mapped, vec3(1/gamma));
	FragColor = vec4(gammaCorrected, 1.0f);
}