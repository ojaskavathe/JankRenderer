#pragma once

#include "Renderer.h"

class TextureFile
{
private:
	unsigned int m_RendererID;
	std::string m_FilePath;
	unsigned char* m_LocalBuffer;
	int m_Width, m_Height, m_BPP;

public:
	TextureFile(
		const std::string& path,
		unsigned int magFilter = GL_LINEAR,
		unsigned int minFilter = GL_LINEAR_MIPMAP_LINEAR,
		unsigned int wrapS = GL_REPEAT,
		unsigned int wrapT = GL_REPEAT
	);
	TextureFile();
	~TextureFile();

	void Bind(unsigned int slot = 0) const;
	void Unbind() const;
	void Delete() const ;

	inline unsigned int GetWidth() const { return m_Width; }
	inline unsigned int GetHeight() const { return m_Height; }
	inline unsigned int GetId() const { return m_RendererID; }
	inline std::string GetPath() const { return  m_FilePath; }
};