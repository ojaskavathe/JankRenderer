#version 460 core

in vec4 FragPos;

uniform vec3 lightPos;
uniform float far_plane;

void main()
{
	float lightDist = length(FragPos.xyz - lightPos);
	gl_FragDepth = lightDist / far_plane; // <- normalizing coordinates to [0, 1]
}