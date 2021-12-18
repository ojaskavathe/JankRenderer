#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, std::vector<Material>& materials)
	: m_Vertices(vertices), m_Indices(indices), m_Materials(materials)
{
	SetupMesh();
}

void Mesh::Draw(Shader& shader, glm::mat4 model, glm::mat4& vp)
{
	glm::mat4 mvp = vp * model;
	glm::mat4 normal = glm::transpose(glm::inverse(model));
	shader.Bind();

	glDisable(GL_CULL_FACE);

	vao.Bind();
	for (unsigned int i = 0; i < m_Materials.size(); ++i)
	{
		Material mat = m_Materials[1];

		std::cout << mat.metallic << std::endl;

		shader.SetUniformMatrix4fv("model", model);
		shader.SetUniformMatrix4fv("mvp", mvp);
		shader.SetUniformMatrix4fv("normalMatrix", normal);

		shader.SetUniform3fv("albedo", mat.albedo);
		shader.SetUniform1f("metallic", mat.metallic);
		shader.SetUniform1f("roughness", mat.roughness);

		glDrawElements(GL_TRIANGLES, unsigned int(m_Indices.size()), GL_UNSIGNED_INT, 0);
	}
	vao.Unbind();

	glEnable(GL_CULL_FACE);

	// always good practice to set everything back to defaults once configured.
	glActiveTexture(GL_TEXTURE0);
	//renderer.DrawElements(va, ib, shader);
}

void Mesh::SetupMesh()
{
	vao.Bind();
	VertexBuffer vbo(&m_Vertices[0], unsigned int(m_Vertices.size()) * sizeof(Vertex));
	IndexBuffer ibo(&m_Indices[0], unsigned int(m_Indices.size()) * sizeof(unsigned int));
	VertexBufferLayout layout;

	layout.Push<float>(3);
	layout.Push<float>(3);
	layout.Push<float>(2);
	vao.AddBuffer(vbo, layout);

	vao.Unbind();
}
