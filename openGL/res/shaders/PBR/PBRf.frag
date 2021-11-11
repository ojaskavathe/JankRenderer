#version 460 core

out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform vec3 camPos;

uniform vec3  albedo;
uniform float metallic;
uniform float roughness;
uniform float ao;

void main()
{
	vec3 N = normalize(Normal);
	vec3 V = normalize(camPos - FragPos);
}