#version 460 core

out vec4 FragColor;

in vec3 Normal;
in vec2 texCoord;
in vec3 FragPosition;
in vec4 lightSpaceFragPos;

uniform vec3 objectColor;
uniform vec3 viewPosition;

struct Material 
{
	vec3 specular;
	float shininess;
};

struct PointLight
{
	vec3 position;

	vec3 color;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	vec3 atten;
};

struct DirectionalLight
{
	vec3 direction;

	vec3 color;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform Material mat;
uniform PointLight pointLight;
uniform DirectionalLight dirLight;

uniform sampler2D shadowMap;
uniform samplerCube shadowCubemap;

//directional light projection near and far plane
uniform float near; 
uniform float far; 

uniform vec4 color;

uniform float oFar; // <- omnidirection shadowmap projection far plane

uniform int halfkernelWidth = 1;

vec3 CalcPointLight(PointLight light, vec3 norm, vec3 viewDir);
vec3 CalcDirLight(DirectionalLight light, vec3 norm, vec3 viewDir);
float CalcDirShadow();
float CalcPointShadow();
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

vec3 norm = normalize(Normal);
void main()
{
	vec3 viewDir = normalize(FragPosition - viewPosition);

	vec3 result = vec3(0);
	result += CalcDirLight(dirLight, norm, viewDir);
	result += CalcPointLight(pointLight, norm, viewDir);
	
	FragColor = vec4(result, 1.0f);
}


vec3 CalcDirLight(DirectionalLight light, vec3 norm, vec3 viewDir)
{
	vec3 lightDir =	normalize(light.direction);
	vec3 reflectDir = (reflect(lightDir, norm));

	vec3 halfwayDir = normalize(-lightDir - viewDir);

	vec3 ambientColor	= light.ambient * color.rgb * light.color;
	vec3 diffuseColor	= max(dot(norm, -lightDir), 0.0f) * light.diffuse * color.rbg * light.color;
	vec3 specularColor	= pow(max(dot(norm, halfwayDir), 0.0f), mat.shininess) * light.specular * light.color;// <- Blinn-Phong

	return ambientColor + (1.0f - CalcDirShadow())*(diffuseColor + specularColor);
	//return ambientColor + (diffuseColor + specularColor);
}

float CalcDirShadow()
{
	vec3 projected = lightSpaceFragPos.xyz / lightSpaceFragPos.w;
	projected = projected * 0.5f + 0.5f;
	//float firstHitDist = texture(shadowMap, projected.xy).r; // -> first fragment hit (to be lit)
	float currentHitDist = projected.z;

	float bias = max(0.001f * (1.0f - dot(norm, -normalize(dirLight.direction))), 0.00f);

	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for(int x = -halfkernelWidth; x <= halfkernelWidth; ++x)
	{
		for(int y = -halfkernelWidth; y <= halfkernelWidth; ++y)
		{
			float pcfDepth = texture(shadowMap, projected.xy + vec2(x, y) * texelSize).r;
			shadow += currentHitDist - bias > pcfDepth ? 0.4 : 0.0;
		}
	}
	shadow /= ((halfkernelWidth*2+1)*(halfkernelWidth*2+1));

	//float shadow = currentHitDist - bias > firstHitDist ? 1.0f : 0.0f;
	if(projected.z > 1.0f) shadow = 0.0f;

	return shadow;
}

vec3 CalcPointLight(PointLight light, vec3 norm, vec3 viewDir)
{
	vec3 lightDir =	normalize(FragPosition - light.position);
	float lightDist = length(FragPosition - light.position);

	vec3 halfwayDir = normalize(-lightDir - viewDir);

	vec3 reflectDir = (reflect(lightDir, norm));
	float attenuation = 1.0f/(light.atten.r + (light.atten.g * lightDist) + (light.atten.b * lightDist * lightDist));

	vec3 ambientColor	= light.ambient * color.rgb * light.color * attenuation;
	vec3 diffuseColor	= max(dot(norm, -lightDir), 0.0f) * light.diffuse * color.rgb * light.color * attenuation;
	vec3 specularColor	= pow(max(dot(norm, halfwayDir), 0.0f), mat.shininess) * light.specular * light.color * attenuation;// <- Blinn-Phong

	return ambientColor + (1.0f - CalcPointShadow())*(diffuseColor + specularColor);
}

float CalcPointShadow()
{
	vec3 lightToFrag = FragPosition - pointLight.position;
	
	float currentHitDist = length(lightToFrag);
	
	float bias = max(0.001f * (1.0f - dot(norm, -normalize(lightToFrag))), 0.00f);;
	//float shadow = currentHitDist - bias > firstHitDist ? 1.0f : 0.0f;

	vec3 sampleOffsetDirections[20] = vec3[]
	(
	   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
	   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
	   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
	   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
	   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
	);   


	//pcf
	float shadow = 0.0;
	int samples  = 20;
	float viewDistance = length(viewPosition - FragPosition);
	float diskRadius = (1.0 + (viewDistance / oFar)) / 100.0;  
	for(int i = 0; i < samples; ++i)
	{
		float firstHitDist = texture(shadowCubemap, lightToFrag + sampleOffsetDirections[i] * diskRadius).r;
		firstHitDist *= oFar;   // undo mapping [0;1]
		if(currentHitDist - bias > firstHitDist)
			shadow += 1.0;
	}
	shadow /= float(samples);  
	
	return shadow;
}
