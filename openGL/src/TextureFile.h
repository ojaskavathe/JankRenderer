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
	TextureFile(const std::string& path);
	~TextureFile();

	void Bind(unsigned int slot = 0) const;
	void Unbind() const;

	inline unsigned int GetWidth() const { return m_Width; }
	inline unsigned int GetHeight() const { return m_Height; }
	inline unsigned int GetId() const { return m_RendererID; }
};