#include "Mesh.h"

Mesh::Mesh(std::vector<Primitive> primitives)
	: m_Primitives(primitives)
{
	SetupMesh();
}

void Mesh::Draw(Shader& shader, glm::mat4 model, glm::mat4& vp)
{
	glm::mat4 mvp = vp * model;
	glm::mat4 normal = glm::transpose(glm::inverse(model));
	shader.Bind();

	glDisable(GL_CULL_FACE);

	//std::cout << m_Primitives.size() << std::endl; 

	for (unsigned int i = 0; i < m_Primitives.size(); ++i)
	{
		m_Primitives[i].vao.Bind();
		Material mat = m_Primitives[i].material;

		shader.SetUniformMatrix4fv("model", model);
		shader.SetUniformMatrix4fv("mvp", mvp);
		shader.SetUniformMatrix4fv("normalMatrix", normal);

		shader.SetUniform3fv("albedo", mat.albedo);
		shader.SetUniform1f("metallic", mat.metallic);
		shader.SetUniform1f("roughness", mat.roughness);

		glDrawElements(GL_TRIANGLES, unsigned int(m_Primitives[i].indices.size()), GL_UNSIGNED_INT, 0);
	}
	
	glBindVertexArray(0);

	glEnable(GL_CULL_FACE);

	// always good practice to set everything back to defaults once configured.
	glActiveTexture(GL_TEXTURE0);
	//renderer.DrawElements(va, ib, shader);
}

void Mesh::SetupMesh()
{
	for (auto i : m_Primitives)
	{
		i.vao.Bind();
		VertexBuffer vbo(&i.vertices[0], unsigned int(i.vertices.size()) * sizeof(Vertex));
		IndexBuffer ibo(&i.indices[0], unsigned int(i.indices.size()) * sizeof(unsigned int));
		VertexBufferLayout layout;

		layout.Push<float>(3);
		layout.Push<float>(3);
		layout.Push<float>(2);
		i.vao.AddBuffer(vbo, layout);

		i.vao.Unbind();
	}
}