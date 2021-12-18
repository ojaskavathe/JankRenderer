#include "Model.h"

Model::Model(const char* path)
{
	std::string text = getFileContents(path);
	JSON = json::parse(text);

	Model::file = path;
	data = getData();

	traverseNode(0);
}

void Model::Draw(Shader& shader, glm::mat4& vp)
{
	for (unsigned int i = 0; i < meshes.size(); ++i)
		meshes[i].Draw(shader, modelMat[i], vp);
}

void Model::loadMesh(unsigned int meshInd)
{
	unsigned int posAccInd = JSON["meshes"][meshInd]["primitives"][0]["attributes"]["POSITION"];
	unsigned int normalAccInd = JSON["meshes"][meshInd]["primitives"][0]["attributes"]["NORMAL"];
	unsigned int texAccInd = JSON["meshes"][meshInd]["primitives"][0]["attributes"]["TEXCOORD_0"];
	unsigned int indAccInd = JSON["meshes"][meshInd]["primitives"][0]["indices"];

	std::vector<float> posVec = getFloats(JSON["accessors"][posAccInd]);
	std::vector<glm::vec3> positions = groupVec3(posVec);
	std::vector<float> normalVec = getFloats(JSON["accessors"][normalAccInd]);
	std::vector<glm::vec3> normals = groupVec3(normalVec);
	std::vector<float> texVec = getFloats(JSON["accessors"][texAccInd]);
	std::vector<glm::vec2> UVs = groupVec2(texVec);

	std::vector<Vertex> vertices = groupVertices(positions, normals, UVs);
	std::vector<unsigned int> indices = getIndices(JSON["accessors"][indAccInd]);
	std::vector<Material> materials = getMaterials();
	meshes.push_back(Mesh(vertices, indices, materials));
}

void Model::traverseNode(unsigned int nextNode, glm::mat4 mat)
{
	json node = JSON["nodes"][nextNode];

	glm::vec3 translation = glm::vec3(0.f);
	if (node.find("translation") != node.end())
	{
		translation.x = node["translation"][0];
		translation.y = node["translation"][1];
		translation.z = node["translation"][2];
	}
		
	glm::quat rotation = glm::quat(1.f, 0.f, 0.f, 0.f);
	if (node.find("rotation") != node.end())
	{
		rotation.x = node["rotation"][3];
		rotation.y = node["rotation"][0];
		rotation.z = node["rotation"][1];
		rotation.w = node["rotation"][2];
	}

	glm::vec3 scale = glm::vec3(1.f);
	if (node.find("scale") != node.end())
	{
		scale.x = node["scale"][0];
		scale.y = node["scale"][1];
		scale.z = node["scale"][2];
	}

	glm::mat4 T = glm::mat4(1.f);
	glm::mat4 R = glm::mat4(1.f);
	glm::mat4 S = glm::mat4(1.f);

	T = glm::translate(T, translation);
	R = glm::mat4_cast(rotation);
	S = glm::scale(S, scale);

	glm::mat4 matNextNode = mat * T * R * S;

	if (node.find("mesh") != node.end())
	{
		modelMat.push_back(T * R * S);
		loadMesh(node["mesh"]);
	}

	if (node.find("children") != node.end())
		std::cout << "hi";
		for (unsigned int i = 0; i < node["children"].size(); ++i)
			traverseNode(node["children"][i], matNextNode);
	if (JSON["nodes"][nextNode + 1].is_object())
		traverseNode(nextNode + 1);
}

std::vector<unsigned char> Model::getData()
{
	std::string bytesText;
	std::string uri = JSON["buffers"][0]["uri"];

	std::string fileStr = std::string(file);
	std::string fileDir = fileStr.substr(0, fileStr.find_last_of('/') + 1); //<-kinda sketch pls fix later
	bytesText = getFileContents((fileDir + uri).c_str());

	std::vector<unsigned char> data(bytesText.begin(), bytesText.end());
	return data;
}

std::vector<float> Model::getFloats(json accessor)
{
	std::vector<float> floatVec;

	unsigned int buffViewIndex = accessor.value("bufferView", 1); //<-value function only returns if the buffer exists, second parameter is backup value in case it doesn't
	unsigned int count = accessor["count"]; //<-doesn't check if count exists
	unsigned int accByteOffset = accessor.value("byteOffset", 0); //<-a lot of files don't have a byteoffset
	std::string type = accessor["type"];

	json bufferView = JSON["bufferViews"][buffViewIndex];
	unsigned int byteOffset = bufferView["byteOffset"];

	unsigned int floatsPerVertex;
	if (type == "SCALAR") floatsPerVertex = 1;
	else if (type == "VEC2") floatsPerVertex = 2;
	else if (type == "VEC3") floatsPerVertex = 3;
	else if (type == "VEC4") floatsPerVertex = 4;
	else throw std::invalid_argument("Invalid type (not SCALAR, VEC2, VEC3, VEC4)");

	unsigned int dataBegin = byteOffset + accByteOffset;
	unsigned int dataLength = bufferView["byteLength"];

	for (unsigned int i = dataBegin; i < dataBegin + dataLength; i) //<- i is being incremented per byte
	{
		//data is a char array, and float->4 bytes
		unsigned char bytes[] = {
			data[i++],
			data[i++],
			data[i++],
			data[i++]
		};
		float value;
		std::memcpy(&value, bytes, sizeof(float));
		floatVec.push_back(value);
	}

	return floatVec;
}

std::vector<unsigned int> Model::getIndices(json accessor)
{
	std::vector<unsigned int> indicesVec;

	unsigned int buffViewIndex = accessor.value("bufferView", 1); //<-value function only returns if the buffer exists, second parameter is backup value in case it doesn't
	unsigned int count = accessor["count"]; //<-doesn't check if count exists
	unsigned int accByteOffset = accessor.value("byteOffset", 0); //<-a lot of files don't have a byteoffset
	unsigned int componentType = accessor["componentType"];

	json bufferView = JSON["bufferViews"][buffViewIndex];
	unsigned int byteOffset = bufferView["byteOffset"];

	unsigned int dataBegin = byteOffset + accByteOffset;
	unsigned int dataLength = bufferView["byteLength"];

	//indices can be either an unsigned int, an unsigned short or a short
	if (componentType == 5125) //int -> 4 bytes
	{
		for (unsigned int i = dataBegin; i < dataBegin + dataLength; i)
		{
			unsigned char bytes[] = {
				data[i++],
				data[i++],
				data[i++],
				data[i++]
			};
			unsigned int value;
			std::memcpy(&value, bytes, sizeof(unsigned int));
			indicesVec.push_back(value);
		}
	}
	else if (componentType == 5123) //unsigned short -> 2 bytes
	{
		for (unsigned int i = dataBegin; i < dataBegin + dataLength; i)
		{
			unsigned char bytes[] = {
				data[i++],
				data[i++]
			};
			unsigned short value;
			std::memcpy(&value, bytes, sizeof(unsigned short));
			indicesVec.push_back(value);
		}
	}
	else if (componentType == 5122) //short -> 2 bytes
	{
		for (unsigned int i = dataBegin; i < dataBegin + dataLength; i)
		{
			unsigned char bytes[] = {
				data[i++],
				data[i++]
			};
			short value;
			std::memcpy(&value, bytes, sizeof(short));
			indicesVec.push_back(value);
		}
	}

	return indicesVec;
}

std::vector<Material> Model::getMaterials()
{
	std::vector<Material> materials;
	json mats = JSON["materials"];

	for (auto it : mats)
	{
		bool skip = false;

		//std::cout << loadedMatName.size() << std::endl;
		for (unsigned int i = 0; i < loadedMatName.size(); ++i)
		{
			if (loadedMatName[i] == it["name"])
			{
				materials.push_back(loadedMat[i]);
				skip = true;
				break;
			}
		}

		glm::vec4 albedo(0.f);
		float metallic = 0.f;
		float roughness = 0.f;

		if (!skip)
		{
			albedo = glm::vec4(
				it["pbrMetallicRoughness"]["baseColorFactor"][0],
				it["pbrMetallicRoughness"]["baseColorFactor"][1],
				it["pbrMetallicRoughness"]["baseColorFactor"][2],
				it["pbrMetallicRoughness"]["baseColorFactor"][3]
			);

			metallic = it["pbrMetallicRoughness"]["metallicFactor"];
			roughness = it["pbrMetallicRoughness"]["roughnessFactor"];

			materials.push_back(Material{ albedo, metallic, roughness });
			loadedMat.push_back(Material{ albedo, metallic, roughness });
			loadedMatName.push_back(it["name"]);
		}

	}

	return materials;
}

std::vector<Vertex> Model::groupVertices
(
	std::vector<glm::vec3> positions,
	std::vector<glm::vec3> normals,
	std::vector<glm::vec2> UVs
)
{
	std::vector<Vertex> vertices;
	for (unsigned int i = 0; i < positions.size(); ++i)
	{
		vertices.push_back(
			Vertex{
				positions[i],
				normals[i],
				UVs[i]
			}
		);
	}

	return vertices;
}

std::vector<glm::vec2> Model::groupVec2(std::vector<float> floatVec)
{
	std::vector<glm::vec2> vectors;
	for (unsigned int i = 0; i < floatVec.size(); i)
		vectors.push_back(glm::vec2(floatVec[i++], floatVec[i++]));
	return vectors;
}

std::vector<glm::vec3> Model::groupVec3(std::vector<float> floatVec)
{
	std::vector<glm::vec3> vectors;
	for (unsigned int i = 0; i < floatVec.size(); i)
		vectors.push_back(glm::vec3(floatVec[i++], floatVec[i++], floatVec[i++]));

	return vectors;
}

std::vector<glm::vec4> Model::groupVec4(std::vector<float> floatVec)
{
	std::vector<glm::vec4> vectors;
	for (unsigned int i = 0; i < floatVec.size(); i)
		vectors.push_back(glm::vec4(floatVec[i++], floatVec[i++], floatVec[i++], floatVec[i++]));
	return vectors;
}

std::string Model::getFileContents(const char* path)
{
	std::ifstream ifs(path);
	std::string text((std::istreambuf_iterator<char>(ifs)),
		(std::istreambuf_iterator<char>()));
	return text;
}