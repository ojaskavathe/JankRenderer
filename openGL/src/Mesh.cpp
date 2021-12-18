#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, std::vector<Texture>& textures, unsigned int &blend)
	: m_Vertices(vertices), m_Indices(indices), m_Textures(textures), m_Blend(blend)
{
	SetupMesh();
}

void Mesh::Draw(Shader &shader)
{
	unsigned int numDiffuse = 1;
	unsigned int numSpecular = 1;

	shader.Bind();

	for (unsigned int i = 0; i < m_Textures.size(); i++)
	{
		std::string num;
		if (m_Textures[i].type == "texture_diffuse")
			num = std::to_string(numDiffuse++);
		if (m_Textures[i].type == "texture_specular")
			num = std::to_string(numSpecular++);

		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_Textures[i].id);	

		//setting uniforms
		shader.SetUniform1i("mat." + m_Textures[i].type + num, i);
	}

	//for models without textures
	/*shader.SetUniform3fv("mat.diffuse", m_Material.diffuse);
	shader.SetUniform3fv("mat.specular", m_Material.specular);
	shader.SetUniform1f("mat.shininess", m_Material.shininess);*/


	//glBindVertexArray(VAO);
	vao.Bind();
	IndexBuffer ibo(&m_Indices[0], unsigned int(m_Indices.size()));
	glDrawElements(GL_TRIANGLES, unsigned int(m_Indices.size()), GL_UNSIGNED_INT, 0);

	// always good practice to set everything back to defaults once configured.
	glActiveTexture(GL_TEXTURE0);
	//renderer.DrawElements(va, ib, shader);
}

void Mesh::SetupMesh()
{
	vao.Bind();
	VertexBuffer vbo(&m_Vertices[0], unsigned int(m_Vertices.size()) * sizeof(Vertex));
	IndexBuffer ibo(&m_Indices[0], unsigned int(m_Indices.size()));
	VertexBufferLayout layout;

	layout.Push<float>(3);
	layout.Push<float>(3);
	layout.Push<float>(2);
	vao.AddBuffer(vbo, layout);

	vao.Unbind();
}
