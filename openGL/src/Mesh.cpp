#include "Mesh.h"

Mesh::Mesh(std::vector<Primitive> primitives)
	: m_Primitives(primitives)
{
	SetupMesh();
}

void Mesh::Draw(Shader& shader, glm::mat4 model, glm::mat4& vp)
{
	glm::mat4 mvp = vp * model;
	glm::mat4 normalMat = glm::transpose(glm::inverse(model));
	shader.Bind();

	glDisable(GL_CULL_FACE);

	for (auto i : m_Primitives)
	{
		i.vao.Bind();
		Material mat = i.material;

		//defaults
		shader.SetUniform1i("hasAlbedoText", 0);
		shader.SetUniform1i("hasMetRoughTex", 0);

		shader.SetUniformMatrix4fv("model", model);
		shader.SetUniformMatrix4fv("mvp", mvp);
		shader.SetUniformMatrix4fv("normalMatrix", normalMat);

		shader.SetUniform4fv("albedo", mat.albedo);
		shader.SetUniform1f("metallicVal", mat.metallic);
		shader.SetUniform1f("roughnessVal", mat.roughness);

		if (i.material.hasAlbedoTex) {
			shader.SetUniform1i("hasAlbedoTex", 1);
			shader.SetUniform1i("albedoTex", 5);
			i.material.albedoTex.Bind(5);
		}

		if (i.material.hasMetRoughTex) {
			shader.SetUniform1i("hasMetRoughTex", 1);
			shader.SetUniform1i("metallicRoughnessTex", 6);
			i.material.metallicRoughnessTex.Bind(6);
		}

		glDrawElements(GL_TRIANGLES, unsigned int(i.indices.size()), GL_UNSIGNED_INT, 0);
	}
	
	glBindVertexArray(0);
	glEnable(GL_CULL_FACE);
	glActiveTexture(GL_TEXTURE0);
}

void Mesh::SetupMesh()
{
	for (auto i : m_Primitives)
	{
		i.vao.Bind();
		VertexBuffer vbo(&i.vertices[0], unsigned int(i.vertices.size()) * sizeof(Vertex));
		IndexBuffer ibo(&i.indices[0], unsigned int(i.indices.size()));
		VertexBufferLayout layout;

		layout.Push<float>(3);
		layout.Push<float>(3);
		layout.Push<float>(2);
		i.vao.AddBuffer(vbo, layout);

		i.vao.Unbind();
	}
}