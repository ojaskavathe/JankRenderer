#pragma once

#include <iostream>
#include <string>

#include <json/json.h>

#include "Shader.h"
#include "Mesh.h"

using json = nlohmann::json;


class Model
{
public:
	Model(const char* path);
	Model(std::vector<Mesh>& meshes);

	void Draw(const Shader& shader, const glm::mat4& vp);
	void Draw(const Shader& shader, const glm::mat4& vp, const glm::mat4& model);

	void DrawShadowMap(const Shader& shader, const glm::mat4& vp);
	void DrawShadowMap(const Shader& shader, const glm::mat4& vp, const glm::mat4& model);

private:
	std::vector<Mesh> m_Meshes;
	std::vector<glm::mat4> m_ModelMat;

	//-------MODEL LOADING-------//
	json JSON;

	std::string m_CurrIndent;

	const char* m_File;
	std::vector<unsigned char> m_Data; //easy to sample floats out of an array of 1 byte chars

	std::vector<Material> m_Materials;
	std::vector<TextureFile> m_Textures;

	void loadMesh(const unsigned int& meshInd);
	Primitive loadPrimitive(const json& prim);

	void traverseNode(const unsigned int& nextNode, const glm::mat4& mat = glm::mat4(1.f));

	std::vector<unsigned char> getData();
	std::vector<float> getFloats(const json& accessor);
	std::vector<unsigned int> getIndices(const json& accessor);
	std::vector<Material> getMaterials();
	std::vector<TextureFile> getTextures();

	std::vector<Vertex> groupVertices(const std::vector<glm::vec3>& positions, const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>& UVs);

	std::vector<glm::vec2> groupVec2(const std::vector<float>& floatVec);
	std::vector<glm::vec3> groupVec3(const std::vector<float>& floatVec);
	std::vector<glm::vec4> groupVec4(const std::vector<float>& floatVec);

	std::string getFileContents(const char* path);
};