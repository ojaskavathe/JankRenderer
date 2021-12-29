#pragma once

class FrameBuffer
{
private:
	unsigned int m_RendererID;
	
public:
	FrameBuffer();
	void GenTextureBuffer(unsigned int& buffer, unsigned int internalFormat, unsigned int format, unsigned int type, unsigned int attachment, unsigned int width = 800, unsigned int height = 600) const;
	void GenTextureBufferMS(unsigned int& buffer, unsigned int internalFormat, unsigned int format, unsigned int type, unsigned int attachment, unsigned int samples) const;
	void attachTextureBuffer(unsigned int attachment, unsigned int& buffer) const;
	void attachTextureBufferMS(unsigned int attachment, unsigned int& buffer) const;
	void GenRenderBuffer(unsigned int& buffer, unsigned int internalFormat, unsigned int format, unsigned int type) const;
	void CheckStatus() const;
	void Bind() const;
	void Unbind() const;
	inline unsigned int getID() const { return m_RendererID;  }
};

