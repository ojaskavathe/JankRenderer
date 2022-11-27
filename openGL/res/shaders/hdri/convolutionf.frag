#version 460 core
out vec4 FragColor;

in vec3 localPos;

uniform samplerCube environmentMap;

const float PI = 3.14159265359;

// Mirror binary digits about the decimal point
float radicalInverse_VdC(uint bits) //Van der Corput sequence
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

// quasi-random(same set of random numbers) sequence that has pretty evenly spaced points
vec2 hammersley(uint i, float oneOverN)
{
    return vec2(float(i) * oneOverN, radicalInverse_VdC(i));
}  

//generate samples biased towards the specular lobe
vec3 importanceSampleGGX(vec2 Xi, vec3 N, float roughness)
{
    float a = roughness * roughness; //disney uses roughness squared

    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y)); //NDF
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

    // from spherical coordinates to cartesian coordinates
    vec3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;

    // from tangent-space vector to world-space sample vector
    vec3 up = abs(N.z) < 0.999 ? vec3 (0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);

    vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
}

void main()
{
    vec3 normal = normalize(localPos);
    vec3 N = normal;

    vec3 irradiance = vec3(0.0);

    const uint SAMPLE_COUNT = 16384u;
    const float oneOverN = 1/float(SAMPLE_COUNT);

    float totalWeight = 0.0;
    for (uint i = 0u; i < SAMPLE_COUNT; ++i) {
        vec2 Xi = hammersley(i, oneOverN);
        vec3 H = importanceSampleGGX(Xi, N, 1.0);

        // NdotH is equal to cos(theta)
        float NoH = max(dot(N, H), 0.0);
        // With roughness == 1 in the distribution function we get 1/pi
        float D = 1.0 / PI;
        float pdf = (D * NoH / (4.0)) + 0.0001; 

        float resolution = 1024.0; // resolution of source cubemap (per face)
        float saTexel = 4.0 * PI / (6.0 * resolution * resolution);
        float saSample = 1.0 / (float(SAMPLE_COUNT) * pdf + 0.0001);

        float mipLevel = 0.5 * log2(saSample / saTexel); 

        irradiance += textureLod(environmentMap, H, mipLevel).rgb * NoH;
        totalWeight += NoH;
    }
    irradiance = (PI * irradiance) / totalWeight;
    
    //if(length(irradiance) > 100.f) irradiance = vec3(0.f, 1.f, 0.f);

    FragColor = vec4(irradiance, 1.0);
}

//void main()
//{
//	vec3 N = normalize(localPos);
//	vec3 irradiance = vec3(0.f);
//
//	vec3 up = vec3(0.f, 1.f, 0.f);
//	vec3 right = normalize(cross(up, N));
//	up = normalize(cross(N, right));
//
//	float delta = 0.025f;
//	int nSamples = 0;
//
//	float totalWeight = 0.f;
//
//	for(float phi = 0.f; phi < 2.f * PI; phi += delta)
//	{
//		for(float theta = 0.f; theta < 0.5f * PI; theta += delta)
//		{
//			//spherical to cartesian (in tangent space)
//			vec3 sampleVec = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
//			//tangent space to world space
//			sampleVec = sampleVec.x * right + sampleVec.y * up + sampleVec.z * N;
//
//			vec3 a = (texture(irradianceMap, normalize(sampleVec)).rgb * cos(theta) * sin(theta));
//            
//            //vec3 a = (textureCube(irradianceMap, normalize(sampleVec)).rgb) * dot(N, sampleVec) / 4 * PI;
//
//            //if(length(a) > 10.f) irradiance += normalize(a)*10.f;
//            irradiance += a;
//			nSamples++;
//			totalWeight += max(dot(N, sampleVec), 0.f);
//		}
//	}
//
//	irradiance = PI * irradiance * (1.f / nSamples);
//	FragColor = vec4(irradiance, 1.f);
//}