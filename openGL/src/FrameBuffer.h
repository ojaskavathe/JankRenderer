#pragma once
#include "VertexBufferLayout.h"
#include "Renderer.h"
#include "Shader.h"

class FrameBuffer
{
private:
	unsigned int m_RendererID;
	
public:
	FrameBuffer();
	void GenTextureBuffer(unsigned int& buffer, unsigned int internalFormat, unsigned int format, GLenum type, GLenum attachment, unsigned int width = WINDOW_WIDTH, unsigned int height = WINDOW_HEIGHT) const;
	void GenTextureBufferMS(unsigned int& buffer, unsigned int internalFormat, unsigned int format, GLenum type, GLenum attachment, unsigned int samples) const;
	void attachTextureBuffer(GLenum attachment, unsigned int& buffer) const;
	void attachTextureBufferMS(GLenum attachment, unsigned int& buffer) const;
	void GenRenderBuffer(unsigned int& buffer, unsigned int internalFormat, unsigned int format, GLenum type) const;
	void CheckStatus() const;
	void Bind() const;
	void Unbind() const;
	inline unsigned int getID() const { return m_RendererID;  }
};

