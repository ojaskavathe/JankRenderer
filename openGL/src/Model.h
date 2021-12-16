#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <numeric>
#include <map>

#include "Shader.h"
#include "Mesh.h"
#include "vendor\stb_image\stb_image.h"

class Model
{
public:
	Model(const std::string& path);

	void Draw(Shader& shader, glm::vec3 camDist);

private:

	std::vector<unsigned int> numFaces;
	std::vector<glm::vec3> facePos;

	std::string directory;

	std::vector<Mesh> meshes;
	std::vector<Mesh> opaqueMeshes;
	std::vector<Mesh> transparentMeshes;

	std::vector<Texture> textures_loaded;
	/*void LoadModel(const std::string& path);
	void ProcessNode(aiNode* node, const aiScene* scene);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);*/
};

