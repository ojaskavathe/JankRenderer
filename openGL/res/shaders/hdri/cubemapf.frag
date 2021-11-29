#version 460 core
out vec4 FragColor;

in vec3 localPos;

uniform sampler2D map;

const vec2 invAtan = vec2(0.1591f, 0.3183f); //vec2(1/2pi, 1/pi)  <- longitude varies over 2pi, latitude over pi
vec2 SampleSphericalMap(vec3 v)
{
	//convert from cartesion to polar
	vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
	
	//convert from polar to uv coords
	uv *= invAtan; //normalizes into -0.5 to 0.5
	uv += 0.5;
	return uv;
}

void main()
{
	vec2 uv = SampleSphericalMap(normalize(localPos));
	vec3 color = texture(map, uv).rgb;

	FragColor = vec4(color, 1.f);
}