#include "FrameBuffer.h"

FrameBuffer::FrameBuffer()
{
	glGenFramebuffers(1, &m_RendererID);
}

void FrameBuffer::GenTextureBuffer(unsigned int& buffer, unsigned int internalFormat, unsigned int format, GLenum type, GLenum attachment)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
	glGenTextures(1, &buffer);
	glBindTexture(GL_TEXTURE_2D, buffer);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, WINDOW_WIDTH, WINDOW_HEIGHT, 0, format, type, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//assigns the texture to framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, buffer, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::GenTextureBufferMS(unsigned int& buffer, unsigned int internalFormat, unsigned int format, GLenum type, GLenum attachment, unsigned int samples)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
	glGenTextures(1, &buffer);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, buffer);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, WINDOW_WIDTH, WINDOW_HEIGHT, GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//assigns the texture to framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D_MULTISAMPLE, buffer, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::attachTextureBuffer(GLenum attachment, unsigned int& buffer)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
	glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, buffer, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::attachTextureBufferMS(GLenum attachment, unsigned int& buffer)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
	glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D_MULTISAMPLE, buffer, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::GenRenderBuffer(unsigned int& buffer, unsigned int internalFormat, unsigned int format, GLenum type)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
	glGenRenderbuffers(1, &buffer);
	glBindRenderbuffer(GL_RENDERBUFFER, buffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);

	//assigns renderbuffer to framebuffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, buffer);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::CheckStatus()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "[FrameBuffer]: Framebuffer not complete." << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
}

void FrameBuffer::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
