#version 460 core

out vec4 FragColor;

in vec3 Normal;
in vec2 texCoord;
in vec3 FragPosition;
in vec4 lightSpaceFragPos;

uniform vec3 objectColor;
uniform vec3 viewPosition;

//uniform samplerCube skybox;

struct Material 
{
	//sampler2D texture_diffuse1;
	//vec3 diffuse;
	//sampler2D texture_specular1;
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

uniform float near; 
uniform float far; 
uniform vec4 color;

//vec4 diffuseTex = texture(mat.texture_diffuse1, texCoord);
//vec4 specularTex = texture(mat.texture_specular1, texCoord);

vec3 CalcPointLight(PointLight light, vec3 norm, vec3 viewDir);
vec3 CalcDirLight(DirectionalLight light, vec3 norm, vec3 viewDir);
float CalcShadow();
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

vec3 norm = normalize(Normal);
void main()
{
	//if(diffuseTex.a < 0.1) discard;


	//float refractiveIndex = 1/1.53;

	vec3 viewDir = normalize(FragPosition - viewPosition);
	//vec3 R = vec3(-reflect(viewDir, norm).x, reflect(viewDir, norm).yz);
	//vec3 R = vec3(-refract(viewDir, norm, refractiveIndex).x, refract(viewDir, norm, refractiveIndex).yz);

	vec3 result = vec3(0);
	//result = texture(skybox, R).rgb;
	result += CalcDirLight(dirLight, norm, viewDir);
	//result += CalcPointLight(pointLight, norm, viewDir);

	FragColor = vec4((result), 1.0f);
}

//for models without textures
vec3 CalcPointLight(PointLight light, vec3 norm, vec3 viewDir)
{
	vec3 lightDir =	normalize(FragPosition - light.position);
	float lightDist = length(FragPosition - light.position);

	vec3 halfwayDir = normalize(-lightDir - viewDir);

	vec3 reflectDir = (reflect(lightDir, norm));
	float attenuation = 1.0f/(light.atten.r + (light.atten.g * lightDist) + (light.atten.b * lightDist * lightDist));

	//vec3 ambientColor	= light.ambient * mat.diffuse * light.color * attenuation;
	vec3 ambientColor	= light.ambient * color.rgb * light.color * attenuation;
	vec3 diffuseColor	= max(dot(norm, -lightDir), 0.0f) * light.diffuse * color.rgb * light.color * attenuation;
	vec3 specularColor	= pow(max(dot(norm, halfwayDir), 0.0f), mat.shininess) * light.specular * light.color * attenuation;// <- Blinn-Phong
	//vec3 specularColor	= pow(max(dot(-viewDir, reflectDir), 0.0f), mat.shininess) * light.specular * light.color * attenuation; // <-Phong

	return ambientColor + diffuseColor + specularColor;
}

vec3 CalcDirLight(DirectionalLight light, vec3 norm, vec3 viewDir)
{
	vec3 lightDir =	normalize(light.direction);
	vec3 reflectDir = (reflect(lightDir, norm));

	vec3 halfwayDir = normalize(-lightDir - viewDir);

	//vec3 ambientColor	= light.ambient * mat.diffuse * light.color;
	vec3 ambientColor	= light.ambient * color.rgb * light.color;
	vec3 diffuseColor	= max(dot(norm, -lightDir), 0.0f) * light.diffuse * color.rbg * light.color;
	vec3 specularColor	= pow(max(dot(norm, halfwayDir), 0.0f), mat.shininess) * light.specular * light.color;// <- Blinn-Phong
	//vec3 specularColor	= pow(max(dot(-viewDir, reflectDir), 0.0f), mat.shininess) * light.specular * light.color; // <- Phong

	return ambientColor + (1.0f - CalcShadow())*(diffuseColor + specularColor);
}

float CalcShadow()
{
	vec3 projected = lightSpaceFragPos.xyz / lightSpaceFragPos.w;
	projected = projected * 0.5f + 0.5f;
	//float firstHitDist = texture(shadowMap, projected.xy).r; // -> first fragment hit (to be lit)
	float currentHitDist = projected.z;

	float bias = max(0.001f * (1.0f - dot(norm, -normalize(dirLight.direction))), 0.00f);

	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	const int halfkernelWidth = 1;
	for(int x = -halfkernelWidth; x <= halfkernelWidth; ++x)
	{
		for(int y = -halfkernelWidth; y <= halfkernelWidth; ++y)
		{
			float pcfDepth = texture(shadowMap, projected.xy + vec2(x, y) * texelSize).r;
			shadow += currentHitDist - bias > pcfDepth ? 1.0 : 0.0;
		}
	}
	shadow /= ((halfkernelWidth*2+1)*(halfkernelWidth*2+1));

	//float shadow = currentHitDist - bias > firstHitDist ? 1.0f : 0.0f;
	if(projected.z > 1.0f) shadow = 0.0f;

	return shadow;
}

////models with textures
//vec3 CalcPointLight(PointLight light, vec3 norm, vec3 viewDir)
//{
//	vec3 lightDir =	normalize(FragPosition - light.position);
//	float lightDist = length(FragPosition - light.position);
//
//	vec3 reflectDir = (reflect(lightDir, norm));
//	float attenuation = 1.0f/(light.atten.r + (light.atten.g * lightDist) + (light.atten.b * lightDist * lightDist));
//
//	vec3 ambientColor	= light.ambient * diffuseTex.rgb * light.color * attenuation;
//	vec3 diffuseColor	= max(dot(norm, -lightDir), 0.0f) * light.diffuse * diffuseTex.rgb * light.color * attenuation;
//	vec3 specularColor	= pow(max(dot(-viewDir, reflectDir), 0.0f), mat.shininess) * light.specular * specularTex.rgb * light.color * attenuation;
//
//	return ambientColor + diffuseColor + specularColor;
//}
//
//
//vec3 CalcDirLight(DirectionalLight light, vec3 norm, vec3 viewDir)
//{
//	vec3 lightDir =	normalize(light.direction);
//	vec3 reflectDir = (reflect(lightDir, norm));
//
//	vec3 ambientColor	= light.ambient * diffuseTex.rgb * light.color;
//	vec3 diffuseColor	= max(dot(norm, -lightDir), 0.0f) * light.diffuse * diffuseTex.rgb * light.color;
//	vec3 specularColor	= pow(max(dot(-viewDir, reflectDir), 0.0f), mat.shininess) * light.specular * specularTex.rgb * light.color;
//
//	return ambientColor + diffuseColor + specularColor;
//}


/*for reflections and refractions using cubemaps
vec3 CalcPointLight(PointLight light, vec3 norm, vec3 viewDir)
{
	vec3 lightDir =	normalize(FragPosition - light.position);
	float lightDist = length(FragPosition - light.position);

	vec3 reflectDir = (reflect(lightDir, norm));
	float attenuation = 1.0f/(light.atten.r + (light.atten.g * lightDist) + (light.atten.b * lightDist * lightDist));

	vec3 ambientColor	= light.ambient * light.color * attenuation;
	vec3 diffuseColor	= max(dot(norm, -lightDir), 0.0f) * light.diffuse * light.color * attenuation;
	vec3 specularColor	= pow(max(dot(-viewDir, reflectDir), 0.0f), mat.shininess) * light.specular * mat.specular * light.color * attenuation;

	return ambientColor + diffuseColor + specularColor;
}

vec3 CalcDirLight(DirectionalLight light, vec3 norm, vec3 viewDir)
{
	vec3 lightDir =	normalize(light.direction);
	vec3 reflectDir = (reflect(lightDir, norm));

	vec3 ambientColor	= light.ambient * light.color;
	vec3 diffuseColor	= max(dot(norm, -lightDir), 0.0f) * light.diffuse * light.color;
	vec3 specularColor	= pow(max(dot(-viewDir, reflectDir), 0.0f), mat.shininess) * light.specular * mat.specular * light.color;

	return ambientColor + diffuseColor + specularColor;
}*/