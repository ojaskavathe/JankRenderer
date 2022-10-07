#include <iostream>
#include "EnvMap.h"
#include <stb_image/stb_image.h>

EnvMap::EnvMap(const char* path)
	:hdriShader("res/shaders/hdri/hdriToCubemapv.vert", "res/shaders/hdri/hdriToCubemapf.frag"),
	cubemapShader("res/shaders/cubemapv.vert", "res/shaders/cubemapf.frag"),
	irradianceShader("res/shaders/hdri/convolutionv.vert", "res/shaders/hdri/convolutionf.frag"),
	prefilterShader("res/shaders/hdri/prefilterv.vert", "res/shaders/hdri/prefilterf.frag"),
	brdfShader("res/shaders/hdri/brdfConvolutionv.vert", "res/shaders/hdri/brdfConvolutionf.frag")
{

	//CUBE
	m_CubeVB = VertexBuffer(m_CubeVertices, (unsigned int)sizeof(m_CubeVertices));
	VertexBufferLayout cubeLayout;
	cubeLayout.Push<float>(3); //position
	cubeLayout.Push<float>(3); //normals
	cubeLayout.Push<float>(2); //texCoords
	m_CubeVA.AddBuffer(m_CubeVB, cubeLayout);
	m_CubeVA.Unbind();
	//-CUBE

	//QUAD
	m_QuadVB = VertexBuffer(m_QuadVertices, (unsigned int)sizeof(m_QuadVertices));
	VertexBufferLayout quadLayout;
	quadLayout.Push<float>(2); //positions
	quadLayout.Push<float>(2); //texCoords
	m_QuadVA.AddBuffer(m_QuadVB, quadLayout);
	m_QuadVA.Unbind();
	//-QUAD

	int width, height, num;
	stbi_set_flip_vertically_on_load(true);
	float* data = stbi_loadf("res/textures/hdri/loft.hdr", &width, &height, &num, 0);
	stbi_set_flip_vertically_on_load(false);

	if (data)
	{
		glGenTextures(1, &m_hdrTex);
		glBindTexture(GL_TEXTURE_2D, m_hdrTex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Failed to load HDR image. \n";
	}

	//framebuffers for environment map
	unsigned int textureRes = 1024;

	glGenFramebuffers(1, &m_envFB);
	glGenRenderbuffers(1, &m_envRB);

	glBindFramebuffer(GL_FRAMEBUFFER, m_envFB);

	glBindRenderbuffer(GL_RENDERBUFFER, m_envRB);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, textureRes, textureRes);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_envRB);

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS); //<-enables interpolation between faces of cubemap

	//environment cubemap texture
	glGenTextures(1, &m_envCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_envCubemap);
	for (int i = 0; i < 6; i++)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, textureRes, textureRes, 0, GL_RGB, GL_FLOAT, nullptr);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//HDRI
	glm::mat4 hdriProj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 hdriVP[] =
	{
	   hdriProj * glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	   hdriProj * glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
	   hdriProj * glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
	   hdriProj * glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
	   hdriProj * glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	   hdriProj * glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};

	hdriShader.Bind();
	hdriShader.SetUniform1i("map", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_hdrTex);

	glViewport(0, 0, textureRes, textureRes);
	glBindFramebuffer(GL_FRAMEBUFFER, m_envFB);

	for (int i = 0; i < 6; i++)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_envCubemap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		hdriShader.SetUniformMatrix4fv("vp", hdriVP[i]);
		m_CubeVA.Bind();
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	m_CubeVA.Unbind();

	glBindTexture(GL_TEXTURE_CUBE_MAP, m_envCubemap);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	//diffuse irradiance map
	glGenTextures(1, &m_irradianceMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_irradianceMap);
	for (int i = 0; i < 6; i++)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB32F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, m_envFB);
	glBindRenderbuffer(GL_RENDERBUFFER, m_envRB);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

	irradianceShader.Bind();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_envCubemap);
	irradianceShader.SetUniform1i("environmentMap", 0);

	glViewport(0, 0, 32, 32);
	glBindFramebuffer(GL_FRAMEBUFFER, m_envFB);

	for (int i = 0; i < 6; i++)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_irradianceMap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		irradianceShader.SetUniformMatrix4fv("vp", hdriVP[i]);
		m_CubeVA.Bind();
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	m_CubeVA.Unbind();

	//prefilter map for specular
	glGenTextures(1, &m_prefilterMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_prefilterMap);
	for (unsigned int i = 0; i < 6; i++)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	prefilterShader.Bind();
	prefilterShader.SetUniform1i("environmentMap", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_envCubemap);

	for (unsigned int mip = 0; mip < 5; ++mip)
	{
		//resize frambuffer according to mip level
		unsigned int mipWidth = (unsigned int)(128 * glm::pow(0.5f, mip));
		unsigned int mipHeight = (unsigned int)(128 * glm::pow(0.5f, mip));

		glBindRenderbuffer(GL_RENDERBUFFER, m_envRB);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
		glViewport(0, 0, mipWidth, mipHeight);

		float mipRoughness = (float)mip / (float)(m_maxMipLevels - 1);
		prefilterShader.SetUniform1f("roughness", mipRoughness);

		for (unsigned int i = 0; i < 6; ++i)
		{
			prefilterShader.SetUniformMatrix4fv("vp", hdriVP[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_prefilterMap, mip);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			m_CubeVA.Bind();
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
	}
	m_CubeVA.Unbind();

	glGenTextures(1, &m_brdfLUT);
	glBindTexture(GL_TEXTURE_2D, m_brdfLUT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, m_envFB);
	glBindRenderbuffer(GL_RENDERBUFFER, m_envRB);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_brdfLUT, 0);

	glViewport(0, 0, 512, 512);
	brdfShader.Bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_QuadVA.Bind();
	glDrawArrays(GL_TRIANGLES, 0, 6);
	m_QuadVA.Unbind();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

EnvMap::~EnvMap()
{
	m_CubeVA.Delete();
	m_CubeVB.Delete();

	m_QuadVA.Delete();
	m_QuadVB.Delete();
}

unsigned int EnvMap::GetEnvCubemap() const
{
	return m_envCubemap;
}

unsigned int EnvMap::GetIrradianceMap() const
{
	return m_irradianceMap;
}

unsigned int EnvMap::GetPrefilterMap() const
{
	return m_prefilterMap;
}

unsigned int EnvMap::GetBRDFLUT() const
{
	return m_brdfLUT;
}
