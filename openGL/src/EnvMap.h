#pragma once

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include "Shader.h"
#include "VertexArray.h"
#include "VertexBuffer.h"

class EnvMap
{
private:
	unsigned int m_hdrTex = 0;
	unsigned int m_envCubemap = 0;
	unsigned int m_irradianceMap = 0;
	unsigned int m_prefilterMap = 0;
	unsigned int m_brdfLUT = 0;
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
	VertexBuffer m_CubeVB;

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
	VertexBuffer m_QuadVB;

	unsigned int m_envFB = 0, m_envRB = 0;

public:
	EnvMap(const char* path);
	~EnvMap();

	unsigned int GetEnvCubemap() const;
	unsigned int GetIrradianceMap() const;
	unsigned int GetPrefilterMap() const;
	unsigned int GetBRDFLUT() const;
};