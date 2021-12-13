#version 460 core
out vec4 FragColor;

in vec3 localPos;

uniform samplerCube irradianceMap;

const float PI = 3.14159265359;

void main()
{
	vec3 N = normalize(localPos);
	vec3 irradiance = vec3(0.f);

	vec3 up = vec3(0.f, 1.f, 0.f);
	vec3 right = normalize(cross(up, N));
	up = normalize(cross(N, right));

	float delta = 0.025f;
	int nSamples = 0;

	float totalWeight = 0.f;

	for(float phi = 0.f; phi < 2.f * PI; phi += delta)
	{
		for(float theta = 0.f; theta < 0.5f * PI; theta += delta)
		{
			//spherical to cartesian (in tangent space)
			vec3 sampleVec = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
			//tangent space to world space
			sampleVec = sampleVec.x * right + sampleVec.y * up + sampleVec.z * N;

			vec3 a = (texture(irradianceMap, normalize(sampleVec)).rgb * cos(theta) * sin(theta));
            
            //vec3 a = (textureCube(irradianceMap, normalize(sampleVec)).rgb) * dot(N, sampleVec) / 4 * PI;

            if(length(a) > 10.f) irradiance += normalize(a)*10.f;
            else irradiance += a;
			nSamples++;
			totalWeight += max(dot(N, sampleVec), 0.f);
		}
	}

	irradiance = PI * irradiance * (1.f / nSamples);
	FragColor = vec4(irradiance, 1.f);
}