#include "TextureFile.h"
#include <stb_image\stb_image.h>

TextureFile::TextureFile(const std::string& path)
	: m_RendererID(0),m_FilePath(path), m_LocalBuffer(NULL), m_Width(0), m_Height(0), m_BPP(0)
{
	glGenTextures(1, &m_RendererID);
	glBindTexture(GL_TEXTURE_2D, m_RendererID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	m_LocalBuffer = stbi_load(path.c_str(), &m_Width, &m_Height, &m_BPP, 0);
	if (m_LocalBuffer)
	{
		unsigned int format;
		switch (m_BPP) {
		case(1):
			format = GL_RED;
			break;

		case(3):
			format = GL_RGB;
			break;

		case(4):
			format = GL_RGBA;
			break;
		}
		
		//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, format, m_Width, m_Height, 0, format, GL_UNSIGNED_BYTE, m_LocalBuffer);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
		stbi_image_free(m_LocalBuffer);
	}
	else
	{
		std::cout << "couldn't load texture" << std::endl;
	}
}

TextureFile::~TextureFile()
{
	//yeah this messes with the way textures are stored, m_RendererID gets reset and multiple textures all get assigned to one id
	//find a way around it
	//glDeleteTextures(1, &m_RendererID);
}

void TextureFile::Bind(unsigned int slot) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_RendererID);
}

void TextureFile::Unbind() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}
