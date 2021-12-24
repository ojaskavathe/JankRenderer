#include "Mesh.h"

Mesh::Mesh(std::vector<Primitive> primitives)
	: m_Primitives(primitives)
{
	SetupMesh();
}

void Mesh::Draw(Shader& shader, glm::mat4& model, glm::mat4& vp)
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
		shader.SetUniform1i("hasAlbedoTex", 0);
		shader.SetUniform1i("hasMetRoughTex", 0);
		shader.SetUniform1i("hasNormalTex", 0);

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

		if (i.material.hasNormalTex) {
			shader.SetUniform1i("hasNormalTex", 1);
			shader.SetUniform1i("normalTex", 7);
			i.material.normalTex.Bind(7);
		}

		glDrawElements(GL_TRIANGLES, unsigned int(i.indices.size()), GL_UNSIGNED_INT, 0);
	}
	
	shader.SetUniform1i("hasAlbedoTex", 0);
	shader.SetUniform1i("hasMetRoughTex", 0);
	shader.SetUniform1i("hasNormalTex", 0);

	glBindVertexArray(0);
	glEnable(GL_CULL_FACE);
	glActiveTexture(GL_TEXTURE0);
}

void Mesh::SetupMesh()
{
	for (auto i : m_Primitives)
	{
		SetupTris(i);
		InitTangentBasis(i);

		i.vao.Bind();

		IndexBuffer ibo(&i.indices[0], unsigned int(i.indices.size()));

		VertexBuffer vbo(&i.vertices[0], unsigned int(i.vertices.size()) * sizeof(Vertex));
		VertexBufferLayout layout;

		layout.Push<float>(3);
		layout.Push<float>(3);
		layout.Push<float>(2);
		layout.Push<float>(3);
		layout.Push<float>(3);

		i.vao.AddBuffer(vbo, layout);

		i.vao.Unbind();
	}
}

void Mesh::SetupTris(Primitive& prim)
{
	for (unsigned int i = 0; i < prim.indices.size() - 2; i+=3)
	{
		Vertex *v1 = &prim.vertices[(prim.indices[i])];
		Vertex *v2 = &prim.vertices[(prim.indices[i + 1])];
		Vertex *v3 = &prim.vertices[(prim.indices[i + 2])];

		prim.Triangles.push_back(Triangle{*v1, *v2, *v3});
	}
}

void Mesh::InitTangentBasis(Primitive& prim)
{
	//for (const auto& j : prim.Triangles) {
	for (unsigned int i = 0; i < prim.Triangles.size(); ++i) {
		glm::vec3 v0 = prim.Triangles[i].v1.position;
		glm::vec3 v1 = prim.Triangles[i].v2.position;
		glm::vec3 v2 = prim.Triangles[i].v3.position;

		glm::vec2 uv0 = prim.Triangles[i].v1.texCoord;
		glm::vec2 uv1 = prim.Triangles[i].v2.texCoord;
		glm::vec2 uv2 = prim.Triangles[i].v3.texCoord;

		// Edges of the triangle : position delta
		glm::vec3 deltaPos1 = v1 - v0;
		glm::vec3 deltaPos2 = v2 - v0;

		// UV delta
		glm::vec2 deltaUV1 = uv1 - uv0;
		glm::vec2 deltaUV2 = uv2 - uv0;

		float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
		glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
		glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

		prim.Triangles[i].v1.tangent = tangent;
		prim.Triangles[i].v2.tangent = tangent;
		prim.Triangles[i].v3.tangent = tangent;

		prim.Triangles[i].v1.bitangent = bitangent;
		prim.Triangles[i].v2.bitangent = bitangent;
		prim.Triangles[i].v3.bitangent = bitangent;

		/*prim.vertices[prim.indices[i]].tangent = tangent;
		prim.vertices[prim.indices[i]].bitangent = bitangent;*/
	}
}
