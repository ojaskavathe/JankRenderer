#pragma once

#include <iostream>

#include <fstream>
#include <iterator>
#include <string>

#include <algorithm>

#include <json/json.h>

#include "Shader.h"
#include "Mesh.h"
#include "vendor\stb_image\stb_image.h"

#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>

using json = nlohmann::json;

class Model
{
public:
	Model(const char* path);

	void Draw(Shader& shader, glm::mat4& vp);

private:

	unsigned int n = 0;
	bool isChild = false;
	std::string currIndent;

	const char* file;
	std::vector<unsigned char> data; //easy to sample floats out of an array of 1 byte chars
	json JSON;

	std::vector<std::string> loadedMatName;
	std::vector<Material> loadedMat;

	std::vector<Mesh> meshes;
	std::vector<glm::mat4> modelMat;

	std::vector<unsigned int> loadedMeshes;

	void loadMesh(unsigned int meshInd);
	Primitive loadPrimitive(json prim);

	void traverseNode(unsigned int nextNode, glm::mat4 mat = glm::mat4(1.f));

	std::vector<unsigned char> getData();
	std::vector<float> getFloats(json accessor);
	std::vector<unsigned int> getIndices(json accessor);
	Material getMaterial(signed int matIndex);

	std::vector<Vertex> groupVertices(std::vector<glm::vec3> positions, std::vector<glm::vec3> normals, std::vector<glm::vec2> UVs);

	std::vector<glm::vec2> groupVec2(std::vector<float> floatVec);
	std::vector<glm::vec3> groupVec3(std::vector<float> floatVec);
	std::vector<glm::vec4> groupVec4(std::vector<float> floatVec);

	std::string getFileContents(const char* path);
};