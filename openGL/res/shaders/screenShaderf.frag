#version 460 core
out vec4 FragColor;

in vec2 texCoords;

uniform sampler2D screenTexture;
uniform float gamma = 2.2f;
uniform float exposure = 0.4f;

uniform int tonemap = 0;

vec3 ACESFilm(vec3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp(((x * (a*x + b)) / (x * (c*x + d) + e)), 0.f, 1.f);
}

void main()
{
	vec3 res = texture(screenTexture, texCoords).rgb;

	vec3 mapped;// = vec3(1.f) - exp(-res * exposure);

    if(tonemap == 0)
	    mapped = vec3(1.f) - exp(-res * exposure);
    else
	    mapped = ACESFilm(res * exposure);

	vec3 gammaCorrected = pow(mapped, vec3(1/gamma));
	FragColor = vec4(gammaCorrected, 1.0f);
}