#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <json/json.h>

#include "Shader.h"
#include "Mesh.h"
#include "vendor\stb_image\stb_image.h"

using json = nlohmann::json;

class Model
{
public:
	Model(const char* path);

	void Draw(Shader& shader, glm::vec3 camDist);

private:

	const char* file;
	std::vector<unsigned char> data; //easy to sample floats out of an array of 1 byte chars
	json JSON;

	std::vector<unsigned char> getData();
	std::vector<float> getFloats(json accessor);
	std::vector<unsigned int> getIndices(json accessor);

	std::vector<Vertex> groupVertices(std::vector<glm::vec3> positions, std::vector<glm::vec3> normals, std::vector<glm::vec2> UVs);

	std::vector<glm::vec2> groupVec2(std::vector<float> floatVec);
	std::vector<glm::vec3> groupVec3(std::vector<float> floatVec);
	std::vector<glm::vec4> groupVec4(std::vector<float> floatVec);

	std::string getFileContents(const char* path);
};

