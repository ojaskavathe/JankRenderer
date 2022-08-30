#pragma once

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include "Renderer.h"

class EnvMap
{
private:
	unsigned int m_hdrTex;
	unsigned int m_envCubemap;
	unsigned int m_irradianceMap;
	unsigned int m_prefilterMap;
	unsigned int m_brdfLUT;
	unsigned int m_maxMipLevels = 5;

	Shader hdriShader;
	Shader cubemapShader;
	Shader irradianceShader;
	Shader prefilterShader;
	Shader brdfShader;

	VertexArray m_CubeVA;
	float m_CubeVertices[288] = {
		//pos				 //normals				//uv
		-1.0f, -1.0, -1.0f,  0.0f,  0.0f, -1.0f,	0.0f, 0.0f,
		 1.0f,  1.0, -1.0f,  0.0f,  0.0f, -1.0f,	1.0f, 1.0f,
		 1.0f, -1.0, -1.0f,  0.0f,  0.0f, -1.0f,	1.0f, 0.0f,
		 1.0f,  1.0, -1.0f,  0.0f,  0.0f, -1.0f,	1.0f, 1.0f,
		-1.0f, -1.0, -1.0f,  0.0f,  0.0f, -1.0f,	0.0f, 0.0f,
		-1.0f,  1.0, -1.0f,  0.0f,  0.0f, -1.0f,	0.0f, 1.0f,

		-1.0f, -1.0,  1.0f,  0.0f,  0.0f,  1.0f,	0.0f, 0.0f,
		 1.0f, -1.0,  1.0f,  0.0f,  0.0f,	1.0f,	1.0f, 0.0f,
		 1.0f,  1.0,  1.0f,  0.0f,  0.0f,	1.0f,	1.0f, 1.0f,
		 1.0f,  1.0,  1.0f,  0.0f,  0.0f,	1.0f,	1.0f, 1.0f,
		-1.0f,  1.0,  1.0f,  0.0f,  0.0f,	1.0f,	0.0f, 1.0f,
		-1.0f, -1.0,  1.0f,  0.0f,  0.0f,	1.0f,	0.0f, 0.0f,

		-1.0f,  1.0,  1.0f, -1.0f,  0.0f,  0.0f,	1.0f, 0.0f,
		-1.0f,  1.0, -1.0f, -1.0f,  0.0f,  0.0f,	1.0f, 1.0f,
		-1.0f, -1.0, -1.0f, -1.0f,  0.0f,  0.0f,	0.0f, 1.0f,
		-1.0f, -1.0, -1.0f, -1.0f,  0.0f,  0.0f,	0.0f, 1.0f,
		-1.0f, -1.0,  1.0f, -1.0f,  0.0f,  0.0f,	0.0f, 0.0f,
		-1.0f,  1.0,  1.0f, -1.0f,  0.0f,  0.0f,	1.0f, 0.0f,

		 1.0f,  1.0,  1.0f,  1.0f,  0.0f,  0.0f,	1.0f, 0.0f,
		 1.0f, -1.0, -1.0f,  1.0f,  0.0f,  0.0f,	0.0f, 1.0f,
		 1.0f,  1.0, -1.0f,  1.0f,  0.0f,  0.0f,	1.0f, 1.0f,
		 1.0f, -1.0, -1.0f,  1.0f,  0.0f,  0.0f,	0.0f, 1.0f,
		 1.0f,  1.0,  1.0f,  1.0f,  0.0f,  0.0f,	1.0f, 0.0f,
		 1.0f, -1.0,  1.0f,  1.0f,  0.0f,  0.0f,	0.0f, 0.0f,

		-1.0f, -1.0, -1.0f,  0.0f, -1.0f,  0.0f,	0.0f, 1.0f,
		 1.0f, -1.0, -1.0f,  0.0f, -1.0f,  0.0f,	1.0f, 1.0f,
		 1.0f, -1.0,  1.0f,  0.0f, -1.0f,  0.0f,	1.0f, 0.0f,
		 1.0f, -1.0,  1.0f,  0.0f, -1.0f,  0.0f,	1.0f, 0.0f,
		-1.0f, -1.0,  1.0f,  0.0f, -1.0f,  0.0f,	0.0f, 0.0f,
		-1.0f, -1.0, -1.0f,  0.0f, -1.0f,  0.0f,	0.0f, 1.0f,

		-1.0f,  1.0, -1.0f,  0.0f,  1.0f,  0.0f,	0.0f, 1.0f,
		 1.0f,  1.0,  1.0f,  0.0f,  1.0f,  0.0f,	1.0f, 0.0f,
		 1.0f,  1.0, -1.0f,  0.0f,  1.0f,  0.0f,	1.0f, 1.0f,
		-1.0f,  1.0,  1.0f,  0.0f,  1.0f,  0.0f,	0.0f, 0.0f,
		 1.0f,  1.0,  1.0f,  0.0f,  1.0f,  0.0f,	1.0f, 0.0f,
		-1.0f,  1.0, -1.0f,  0.0f,  1.0f,  0.0f, 	0.0f, 1.0f
	};

	VertexArray m_QuadVA;
	float m_QuadVertices[24] = {
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};

	unsigned int m_envFB, m_envRB;

public:
	EnvMap(const char* path);
	~EnvMap();

	unsigned int GetEnvCubemap() const;
	unsigned int GetIrradianceMap() const;
	unsigned int GetPrefilterMap() const;
	unsigned int GetBRDFLUT() const;
};