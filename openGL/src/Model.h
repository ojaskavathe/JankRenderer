#pragma once

#include <vector>
#include <glm\gtc\matrix_transform.hpp>

#include "Mesh.h" 

class Shader;
class Mesh;

class Model
{
public:
	Model(const Model& model);
	Model(Model&& model) noexcept;
	Model(std::vector<Mesh>& meshes);
	Model(std::vector<Mesh>& meshes, std::vector<glm::mat4>& modelMat);

	void Draw(const Shader& shader, const glm::mat4& vp);
	void Draw(const Shader& shader, const glm::mat4& vp, const glm::mat4& model);

	void DrawShadowMap(const Shader& shader, const glm::mat4& vp);
	void DrawShadowMap(const Shader& shader, const glm::mat4& vp, const glm::mat4& model);

private:
	std::vector<Mesh> m_Meshes;
	std::vector<glm::mat4> m_ModelMat;
};