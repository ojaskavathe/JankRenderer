#version 460 core

layout (location = 0) out vec4 accum;
layout (location = 1) out float reveal;

uniform vec4 color;

in vec3 FragPosition;
in vec2 texCoord;
in vec3 Normal;

uniform vec3 objectColor;
uniform vec3 viewPosition;

uniform samplerCube skybox;

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

uniform Material mat;
uniform PointLight pointLight;
//vec4 diffuseTex = texture(mat.texture_diffuse1, texCoord);
//vec4 specularTex = texture(mat.texture_specular1, texCoord);

vec3 CalcPointLight(PointLight light, vec3 norm, vec3 viewDir);

uniform float near; 
uniform float far; 
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main()
{
	//if(diffuseTex.a < 0.1) discard;

	vec3 norm = normalize(Normal);

	//float refractiveIndex = 1/1.53;

	vec3 viewDir = normalize(FragPosition - viewPosition);
	//vec3 R = vec3(-reflect(viewDir, norm).x, reflect(viewDir, norm).yz);
	//vec3 R = vec3(-refract(viewDir, norm, refractiveIndex).x, refract(viewDir, norm, refractiveIndex).yz);

	vec3 result = vec3(0);
	//result = texture(skybox, R).rgb;
	result += CalcPointLight(pointLight, norm, viewDir);

	float weight = clamp(pow(min(1.0, color.a * 10.0) + 0.01, 3.0) * 1e8 * pow(1.0 - gl_FragCoord.z * 0.9, 3.0), 1e-2, 3e3);

	accum = vec4(result.rgb * color.a, color.a) * weight;

	reveal = color.a;
}

vec3 CalcPointLight(PointLight light, vec3 norm, vec3 viewDir)
{
	vec3 lightDir =	normalize(FragPosition - light.position);
	float lightDist = length(FragPosition - light.position);

	vec3 reflectDir = (reflect(lightDir, norm));
	float attenuation = 1.0f/(light.atten.r + (light.atten.g * lightDist) + (light.atten.b * lightDist * lightDist));

	vec3 ambientColor	= light.ambient * color.rgb * light.color * attenuation;
	vec3 diffuseColor	= max(dot(norm, -lightDir), 0.0f) * light.diffuse * color.rgb * light.color * attenuation;
	vec3 specularColor	= pow(max(dot(-viewDir, reflectDir), 0.0f), mat.shininess) * light.specular * light.color * attenuation;

	//return diffuseColor;
	return ambientColor + diffuseColor + specularColor;
}

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

/*for models without textures
vec3 CalcPointLight(PointLight light, vec3 norm, vec3 viewDir)
{
	vec3 lightDir =	normalize(FragPosition - light.position);
	float lightDist = length(FragPosition - light.position);

	vec3 reflectDir = (reflect(lightDir, norm));
	float attenuation = 1.0f/(light.atten.r + (light.atten.g * lightDist) + (light.atten.b * lightDist * lightDist));

	vec3 ambientColor	= light.ambient *mat.diffuse * light.color * attenuation;
	vec3 diffuseColor	= max(dot(norm, -lightDir), 0.0f) * light.diffuse * mat.diffuse * light.color * attenuation;
	vec3 specularColor	= pow(max(dot(-viewDir, reflectDir), 0.0f), mat.shininess) * light.specular * mat.specular * light.color * attenuation;

	return ambientColor + diffuseColor + specularColor;
}

for models without textures
vec3 CalcDirLight(DirectionalLight light, vec3 norm, vec3 viewDir)
{
	vec3 lightDir =	normalize(light.direction);
	vec3 reflectDir = (reflect(lightDir, norm));

	vec3 ambientColor	= light.ambient * mat.diffuse * light.color;
	vec3 diffuseColor	= max(dot(norm, -lightDir), 0.0f) * light.diffuse * mat.diffuse * light.color;
	vec3 specularColor	= pow(max(dot(-viewDir, reflectDir), 0.0f), mat.shininess) * light.specular * mat.specular * light.color;

	return ambientColor + diffuseColor + specularColor;
}*/

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