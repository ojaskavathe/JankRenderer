#pragma once
#include "VertexBufferLayout.h"
#include "Renderer.h"
#include "Shader.h"

class FrameBuffer
{
private:
	unsigned int m_RendererID;
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
	void GenTextureBuffer(unsigned int& buffer, unsigned int internalFormat, unsigned int format, GLenum type, GLenum attachment);
	void attachTextureBuffer(GLenum attachment, unsigned int& buffer);
	void GenRenderBuffer(unsigned int& buffer, unsigned int internalFormat, unsigned int format, GLenum type);
	void CheckStatus();
	void Bind();
	void Unbind();
	//void Render(Shader &shader);
	inline unsigned int getID() const { return m_RendererID;  }
};

