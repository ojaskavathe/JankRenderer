#include "Cubemap.h"

#include <glad/gl.h>
#include <iostream>
#include <stb_image/stb_image.h>

#include <vector>
#include <string>
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "Shader.h"


Cubemap::Cubemap()
{
	glGenTextures(1, &m_RendererID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);
}

void Cubemap::Load(std::vector<std::string>& faces)
{
	stbi_set_flip_vertically_on_load(false);

	glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);
	int width, height, nChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else 
		{
			std::cout << "[Cubemap]: Failed to load texture at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}

	stbi_set_flip_vertically_on_load(true);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	
	//works with nvidia but not with radeon

	vao.Bind();
	VertexBuffer vb(skyboxVerts, unsigned int(sizeof(skyboxVerts)));
	VertexBufferLayout layout;
	layout.Push<float>(3);
	vao.AddBuffer(vb, layout);
	vao.Unbind();
}

void Cubemap::Render(Shader &shader)
{
	glDepthFunc(GL_LEQUAL);
	vao.Bind();
	shader.Bind();
	shader.SetUniform1i("box", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDepthFunc(GL_LESS);
}
