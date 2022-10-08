#include <glad/glad.h>
#include <iostream>
#include <stb_image\stb_image.h>
#include "TextureFile.h"

TextureFile::TextureFile(
	const std::string& path,
	unsigned int _magFilter,
	unsigned int _minFilter,
	unsigned int _wrapS,
	unsigned int _wrapT
)
	: m_RendererID(0), m_FilePath(path), m_LocalBuffer(NULL), m_Width(0), m_Height(0), m_BPP(0),
	m_MagFilter(_magFilter),
	m_MinFilter(_minFilter),
	m_WrapS(_wrapS),
	m_WrapT(_wrapT)
{
	SetupTexture();
}

//TextureFile::TextureFile(const TextureFile& textureFile)
//	:m_RendererID(0), m_FilePath(textureFile.m_FilePath), m_LocalBuffer(NULL), m_Width(0), m_Height(0), m_BPP(0),
//	m_MagFilter(textureFile.m_MagFilter),
//	m_MinFilter(textureFile.m_MinFilter),
//	m_WrapS(textureFile.m_WrapS),
//	m_WrapT(textureFile.m_WrapT)
//{
//	SetupTexture();
//}
//
//TextureFile::TextureFile(TextureFile&& textureFile) noexcept
//	:m_RendererID(0), m_FilePath(textureFile.m_FilePath), m_LocalBuffer(NULL), m_Width(0), m_Height(0), m_BPP(0),
//	m_MagFilter(textureFile.m_MagFilter),
//	m_MinFilter(textureFile.m_MinFilter),
//	m_WrapS(textureFile.m_WrapS),
//	m_WrapT(textureFile.m_WrapT)
//{
//	SetupTexture();
//}

TextureFile::TextureFile()
{
}

void TextureFile::SetupTexture()
{
	glGenTextures(1, &m_RendererID);
	glBindTexture(GL_TEXTURE_2D, m_RendererID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_MagFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_MinFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_WrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_WrapT);

	m_LocalBuffer = stbi_load(m_FilePath.c_str(), &m_Width, &m_Height, &m_BPP, 0);
	if (m_LocalBuffer)
	{
		unsigned int format = GL_RGB;
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
		std::cout << "[TextureFile]: Couldn't load texture: " << m_FilePath << "\n";
	}
}

TextureFile::~TextureFile()
{
	//FIX PLS
	//Textures have the same problem as VA's where they need to last longer than their scope
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

void TextureFile::Delete() const
{
	glDeleteTextures(1, &m_RendererID);
}
