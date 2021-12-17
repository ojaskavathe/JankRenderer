#version 460 core

out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform vec3 camPos;

uniform vec3 albedo;
uniform float metallic;
uniform float roughness;
uniform float ao;
uniform float iblIntensity;

uniform vec3 pointLightPos;
uniform vec3 dirLightDir;
uniform vec3 pointLightColor;
uniform vec3 dirLightColor;

uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D   brdfLUT;  

vec3 FresnelSchlick(float HoV, vec3 F0);
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometryGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);

vec3 calcPointLight(vec3 lightPosition);
vec3 calcDirLight(vec3 lightDir);

const float PI = 3.14159265359;

vec3 N = normalize(Normal);
vec3 V = normalize(camPos - FragPos);

vec3 F0 = vec3(0.04);

void main()
{
	F0 = mix(F0, albedo, metallic);

	vec3 L0 = vec3(0.0); // <- total outgoing radiance or irradiance
	
	//L0 += calcPointLight(pointLightPos); 
	//L0 += calcDirLight(dirLightDir); 
	
	vec3 R = reflect(-V, N);

	vec3 F = FresnelSchlick(max(dot(N, V), 0.0), F0);

	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;
	kD *= 1.0 - metallic;	

	vec3 irradiance = texture(irradianceMap, N).rgb;
	vec3 diffuse = irradiance * albedo;

	const float MAX_REFLECT_LOD = 4.f;
	vec3 prefilteredColor = textureLod(prefilterMap, R, roughness * MAX_REFLECT_LOD).rgb;
	vec2 envBRDF = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
	vec3 specular = prefilteredColor * (F * envBRDF.x + envBRDF.y);

	vec3 ambient = (kD * diffuse + specular) * ao;

	vec3 color = (iblIntensity*ambient) + L0;

	//color = color / (color + vec3(1.0)); // <- HDR using reinhart
	//color = pow(color, vec3(1.0/2.2)); // <- i do the gamma correction in the final screenshader
	FragColor = vec4(color, 1.0f);
}

vec3 calcPointLight(vec3 lightPosition)
{
	vec3 res;
	vec3 L = normalize(lightPosition - FragPos);
	vec3 H = normalize(V + L);
	float lightDist = length(lightPosition - FragPos);
	float attenuation = 1 / (lightDist * lightDist);
	vec3 radiance = pointLightColor * attenuation;

	float NDF = DistributionGGX(N, H, roughness);       
	float G = GeometrySmith(N, V, L, roughness);
	vec3 F = FresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0); // <- technically fresnel uses n dot v but ue4 uses h dot v and it kinda looks better so yeah
	
	vec3 numerator = NDF * G * F;
	float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0)  + 0.0001;
	vec3 specular = numerator / denominator;  

	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;
  
	kD *= 1.0 - metallic;

	float NoL = max(dot(N, L), 0.0);
	
	res = (kD * albedo / PI + specular) * radiance * NoL;
	return res;
}

vec3 calcDirLight(vec3 lightDir)
{
	vec3 res;
	vec3 L = -1*normalize(lightDir);
	vec3 H = normalize(V + L);
	
	vec3 radiance = dirLightColor;

	float NDF = DistributionGGX(N, H, roughness);       
	float G = GeometrySmith(N, V, L, roughness);
	vec3 F = FresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0); // <- technically fresnel uses n dot v but ue4 uses h dot v and it kinda looks better so yeah
	
	vec3 numerator = NDF * G * F;
	float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0)  + 0.0001;
	vec3 specular = numerator / denominator;  

	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;
  
	kD *= 1.0 - metallic;

	float NoL = max(dot(N, L), 0.0);
	
	res = (kD * albedo / PI + specular) * radiance * NoL;
	return res;
}

vec3 FresnelSchlick(float HoV, vec3 F0)
{
	//return F0 + (1.0 - F0) * pow(clamp(1.0 - HdotV, 0.0, 1.0), 5.0);
	return F0 + (max(vec3(1.f - roughness), F0) - F0) * pow(clamp(1.0 - HoV, 0.0, 1.0), 5.0); // <- takes in account roughness
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a      = roughness*roughness;
    float a2     = a*a;
    float NoH  = max(dot(N, H), 0.0);
    float NoH2 = NoH*NoH;
	
    float num   = a2;
    float denom = (NoH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float GeometrySchlickGGX(float NoV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NoV;
    float denom = NoV * (1.0 - k) + k;
	
    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NoV = max(dot(N, V), 0.0);
    float NoL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NoV, roughness);
    float ggx1  = GeometrySchlickGGX(NoL, roughness);
	
    return ggx1 * ggx2;
}