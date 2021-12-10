#version 460

in vec3 localPos;

out vec4 FragColor;

uniform samplerCube environmentMap;
uniform float roughness;

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

float DistributionGGX(float NoH, float roughness)
{
	float a      = roughness*roughness;
    float a2     = a*a;
    float NoH2 = NoH*NoH;
	
    float num   = a2;
    float denom = (NoH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

void main()
{
    vec3 N = normalize(localPos);
    vec3 R = N;
    vec3 V = R;

    float resolution = 2048.f;

    vec3 prefilteredColor = vec3(0.f);

    const uint SAMPLE_COUNT = 4096; //2^14
    const float oneOverN = 1/SAMPLE_COUNT;
    float totalWeight = 0.0;
    for (uint i = 0u; i < SAMPLE_COUNT; ++i) {
        vec2 Xi = hammersley(i, oneOverN);
        vec3 H = importanceSampleGGX(Xi, N, roughness);
        vec3 L = 2 * dot( V, H ) * H - V;


        float NoL = max(dot(N, L), 0);
        if( NoL > 0 )
        {
            float NoH = max(dot(N, H), 0);
            float HoV = max(dot(H, V), 0);
            float D = DistributionGGX(NoH, roughness);
            float pdf = D * NoH / ((4.f * HoV) + 0.0001);

            float saTexel = 4.f * PI / (6.f * resolution * resolution);
            float saSample = 1.f / (float(SAMPLE_COUNT) * pdf + 0.0001);

            float mipLevel = (roughness == 0.f) ? 0.f : 0.5 * log2(saSample / saTexel);

            prefilteredColor += textureLod(environmentMap, L, mipLevel).rgb * NoL;
            totalWeight += NoL;
        }
    }
    
    prefilteredColor = prefilteredColor / totalWeight;

    FragColor = vec4(prefilteredColor, 1.0);
}