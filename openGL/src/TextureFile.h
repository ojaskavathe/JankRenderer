#pragma once
#include <string>

class TextureFile
{
private:
	unsigned int m_RendererID = 0;
	std::string m_FilePath;
	unsigned char* m_LocalBuffer = NULL;
	int m_Width = 0, m_Height = 0, m_BPP = 0;

public:
	TextureFile(
		const std::string& path,
		unsigned int magFilter = 0x2601, //GL_LINEAR
		unsigned int minFilter = 0X2703, //GL_LINEAR_MIPMAP_LINEAR
		unsigned int wrapS = 0x2901, //GL_REPEAT
		unsigned int wrapT = 0x2901 //GL_REPEAT
	);
	//TextureFile(const TextureFile& textureFile);
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