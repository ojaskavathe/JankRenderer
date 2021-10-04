#pragma once
#include "VertexBufferLayout.h"
#include "Renderer.h"
#include "Shader.h"

class FrameBuffer
{
private:
	unsigned int m_RendererID;
	unsigned int texColorBuffer;
	unsigned int rbo;
	VertexArray quadVA;

	//quad to feed texture
	float quadVerts[24] = {
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};
	
public:
	FrameBuffer();
	void GenTextureBuffer();
	void GenRenderBuffer();
	void CheckStatus();
	void Write();
	void Unbind();
	void Render(Shader &shader);
};

