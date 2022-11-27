#include "Model.h"

#include <stb_image/stb_image.h>

#include "Shader.h"
#include "Mesh.h"

#include "loaders/GLTFLoader.h"

Model::Model(const Model& model)
	:m_Meshes(model.m_Meshes), m_ModelMat(model.m_ModelMat)
{
}

Model::Model(Model&& model) noexcept
	:m_Meshes(model.m_Meshes), m_ModelMat(model.m_ModelMat)
{
}

Model::Model(std::vector<Mesh>& meshes)
	:m_Meshes(meshes)
{
	m_ModelMat.reserve(m_Meshes.size());
	for (unsigned int i = 0; i < m_Meshes.size(); ++i)
		m_ModelMat.push_back(glm::mat4(1.f));
}

Model::Model(std::vector<Mesh>& meshes, std::vector<glm::mat4>& modelMat)
	:m_Meshes(meshes), m_ModelMat(modelMat)
{
}

//-----------Drawing------------//

void Model::Draw(const Shader& shader, const glm::mat4& vp)
{
	for (unsigned int i = 0; i < m_Meshes.size(); ++i)
		m_Meshes[i].Draw(shader, m_ModelMat[i], vp);
}

void Model::Draw(const Shader& shader, const glm::mat4& vp, const glm::mat4& model)
{
	for (unsigned int i = 0; i < m_Meshes.size(); ++i)
		m_Meshes[i].Draw(shader, (m_ModelMat[i] * model), vp);
}

void Model::DrawShadowMap(const Shader& shader, const glm::mat4& vp)
{
	for (unsigned int i = 0; i < m_Meshes.size(); ++i)
		m_Meshes[i].DrawShadowMap(shader, m_ModelMat[i]);
}

void Model::DrawShadowMap(const Shader& shader, const glm::mat4& vp, const glm::mat4& model)
{
	for (unsigned int i = 0; i < m_Meshes.size(); ++i)
		m_Meshes[i].DrawShadowMap(shader, (m_ModelMat[i] * model));
}