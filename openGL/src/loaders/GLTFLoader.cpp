#include <fstream>
#include <iterator> //for getFileContents
#include <string>
#include <algorithm> //for std::remove

#include <iostream>
#include <string>

#include <json/json.h>

#include "Shader.h"
#include "Mesh.h"

#include <stb_image/stb_image.h>
#include <glm/gtc/quaternion.hpp>

#include "loaders/GLTFLoader.h"

//using an anonymous namespace so as to encapsule the GLTFLoader class
namespace {
	class GLTFLoader
	{
	public:
		Model Load(const char* path);

	private:
		using json = nlohmann::json;
		std::vector<Mesh> m_Meshes;
		std::vector<glm::mat4> m_ModelMat;
		json JSON;
		std::string m_CurrIndent;
		const char* m_File;
		std::vector<unsigned char> m_Data; //easy to sample floats out of an array of 1 byte chars
		std::vector<Material> m_Materials;
		std::vector<TextureFile> m_Textures;

		void loadMesh(const unsigned int& meshInd);
		Primitive loadPrimitive(const json& prim);

		void traverseNode(const unsigned int& nextNode, const glm::mat4& mat = glm::mat4(1.f));

		std::vector<unsigned char>	getData();
		std::vector<TextureFile>	getTextures();
		std::vector<Material>		getMaterials();
		std::vector<float>			getFloats(const json& accessor);
		std::vector<unsigned int>	getIndices(const json& accessor);

		std::vector<Vertex> groupVertices(const std::vector<glm::vec3>& positions, const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>& UVs);

		std::vector<glm::vec2> groupVec2(const std::vector<float>& floatVec);
		std::vector<glm::vec3> groupVec3(const std::vector<float>& floatVec);
		std::vector<glm::vec4> groupVec4(const std::vector<float>& floatVec);

		std::string getFileContents(const char* path);
	};
}

namespace Load {
	Model GLTF(const char* path)
	{
		GLTFLoader loader;
		return loader.Load(path);
	}
}

Model GLTFLoader::Load(const char* path)
{
	std::string text = getFileContents(path);
	JSON = json::parse(text);

	m_File = path;
	m_Data = getData();

	if (JSON["textures"].is_array()) m_Textures = getTextures();
	if (JSON["materials"].is_array()) m_Materials = getMaterials();

	traverseNode((unsigned int)JSON["nodes"].size() - 1);

	return Model(m_Meshes, m_ModelMat);
}

//------------MODEL LOADING--------------//

void GLTFLoader::loadMesh(const unsigned int& meshInd)
{
	std::vector<Primitive> primitives;
	json prims = JSON["meshes"][meshInd]["primitives"];
	primitives.reserve(prims.size());

	for (auto& i : prims)
		primitives.push_back(loadPrimitive(i));

	m_Meshes.emplace_back(primitives);
}

Primitive GLTFLoader::loadPrimitive(const json& prim)
{
	unsigned int posAccInd = prim["attributes"]["POSITION"];
	unsigned int normalAccInd = prim["attributes"]["NORMAL"];
	unsigned int texAccInd = prim["attributes"]["TEXCOORD_0"];
	unsigned int indAccInd = prim["indices"];
	signed int matInd = prim.value("material", -1);

	std::vector<float> posVec = getFloats(JSON["accessors"][posAccInd]);
	std::vector<glm::vec3> positions = groupVec3(posVec);
	std::vector<float> normalVec = getFloats(JSON["accessors"][normalAccInd]);
	std::vector<glm::vec3> normals = groupVec3(normalVec);
	std::vector<float> texVec = getFloats(JSON["accessors"][texAccInd]);
	std::vector<glm::vec2> UVs = groupVec2(texVec);

	std::vector<Vertex> vertices = groupVertices(positions, normals, UVs);
	std::vector<unsigned int> indices = getIndices(JSON["accessors"][indAccInd]);

	Material material;
	if (matInd != -1) material = m_Materials[matInd];

	return Primitive(vertices, indices, material);
}

void GLTFLoader::traverseNode(const unsigned int& nextNode, const glm::mat4& mat)
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
		rotation.w = node["rotation"][3];
		rotation.x = node["rotation"][0];
		rotation.y = node["rotation"][1];
		rotation.z = node["rotation"][2];
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
		m_ModelMat.push_back(matNextNode);
		loadMesh(node["mesh"]);
	}

	if (node["children"].is_array()) {
		//SUPER SKETCH PLSSSS FIX
		std::cout << m_CurrIndent;
		m_CurrIndent += "        ";
		std::string name = node["name"];
		std::cout << name << "\n";
		for (unsigned int i = 0; i < node["children"].size(); ++i) {
			unsigned int childNum = node["children"][i];
			if (!(JSON["nodes"][childNum]["children"].is_array())) {
				std::string childName = JSON["nodes"][childNum]["name"];
				std::cout << m_CurrIndent;
				std::cout << childName << "\n";
			}
			traverseNode(node["children"][i], matNextNode);
		}
		m_CurrIndent.erase(std::remove(m_CurrIndent.end() - 8, m_CurrIndent.end(), ' '), m_CurrIndent.end());
	}
	/*else if (JSON["nodes"][(signed int)nextNode - 1].is_object()) {
		isChild = false;
		traverseNode(nextNode - 1u);
	}*/
}

std::vector<unsigned char> GLTFLoader::getData()
{
	std::string bytesText;
	std::string uri = JSON["buffers"][0]["uri"];

	std::string fileStr = std::string(m_File);
	std::string fileDir = fileStr.substr(0, fileStr.find_last_of('/') + 1); //<-kinda sketch pls fix later

	//can't use getFileContents here as we need binary data
	std::ifstream input((fileDir + uri).c_str(), std::ios::binary);
	std::vector<unsigned char> data(std::istreambuf_iterator<char>(input), {});

	return data;
}

std::vector<float> GLTFLoader::getFloats(const json& accessor)
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

	floatVec.reserve(dataBegin + dataLength);

	for (unsigned int i = dataBegin; i < dataBegin + dataLength; i) //<- i is being incremented per byte
	{
		//data is a char array, and float->4 bytes
		unsigned char bytes[] = {
			m_Data[i++],
			m_Data[i++],
			m_Data[i++],
			m_Data[i++]
		};
		//this seems kinda sketchy, might be an issue with ordering, but it works for now

		float value;
		std::memcpy(&value, &bytes, sizeof(float));
		floatVec.emplace_back(value);
	}

	return floatVec;
}

std::vector<unsigned int> GLTFLoader::getIndices(const json& accessor)
{
	std::vector<unsigned int> indicesVec;

	unsigned int buffViewIndex = accessor.value("bufferView", 1);
	unsigned int count = accessor["count"];
	unsigned int accByteOffset = accessor.value("byteOffset", 0);
	unsigned int componentType = accessor["componentType"];

	json bufferView = JSON["bufferViews"][buffViewIndex];
	unsigned int byteOffset = bufferView["byteOffset"];

	unsigned int dataBegin = byteOffset + accByteOffset;
	unsigned int dataLength = bufferView["byteLength"];

	indicesVec.reserve(dataBegin + dataLength);

	//indices can be either an unsigned int, an unsigned short or a short
	if (componentType == 5125) //int -> 4 bytes
	{
		for (unsigned int i = dataBegin; i < dataBegin + dataLength; i)
		{
			unsigned char bytes[] = {
				m_Data[i++],
				m_Data[i++],
				m_Data[i++],
				m_Data[i++]
			};
			unsigned int value;
			std::memcpy(&value, &bytes, sizeof(unsigned int));
			indicesVec.emplace_back(value);
		}
	}
	else if (componentType == 5123) //unsigned short -> 2 bytes
	{
		for (unsigned int i = dataBegin; i < dataBegin + dataLength; i)
		{
			unsigned char bytes[] = {
				m_Data[i++],
				m_Data[i++]
			};
			unsigned short value;
			std::memcpy(&value, &bytes, sizeof(unsigned short));
			indicesVec.emplace_back(value);
		}
	}
	else if (componentType == 5122) //short -> 2 bytes
	{
		for (unsigned int i = dataBegin; i < dataBegin + dataLength; i)
		{
			unsigned char bytes[] = {
				m_Data[i++],
				m_Data[i++]
			};
			short value;
			std::memcpy(&value, &bytes, sizeof(short));
			indicesVec.emplace_back(value);
		}
	}

	return indicesVec;
}

std::vector<Material> GLTFLoader::getMaterials()
{
	std::vector<Material> materials;
	materials.reserve(JSON["materials"].size());

	for (auto i : JSON["materials"])
	{
		Material material;

		if (i["pbrMetallicRoughness"]["baseColorTexture"].is_object()) {
			unsigned int texIndex = i["pbrMetallicRoughness"]["baseColorTexture"]["index"];
			material.albedoTex = m_Textures[texIndex];
			material.hasAlbedoTex = true;
		}
		else if (i["pbrMetallicRoughness"]["baseColorFactor"].is_array()) {
			glm::vec4 albedo = glm::vec4(
				i["pbrMetallicRoughness"]["baseColorFactor"][0],
				i["pbrMetallicRoughness"]["baseColorFactor"][1],
				i["pbrMetallicRoughness"]["baseColorFactor"][2],
				i["pbrMetallicRoughness"]["baseColorFactor"][3]
			);
			material.albedo = albedo;
		}

		if (i["pbrMetallicRoughness"]["metallicRoughnessTexture"].is_object()) {
			unsigned int texIndex = i["pbrMetallicRoughness"]["metallicRoughnessTexture"]["index"];
			material.metallicRoughnessTex = m_Textures[texIndex];
			material.metallic = i["pbrMetallicRoughness"].value("metallicFactor", 1.f);
			material.roughness = i["pbrMetallicRoughness"].value("roughnessFactor", 1.f);
			material.hasMetRoughTex = true;
		}
		else {
			material.metallic = i["pbrMetallicRoughness"].value("metallicFactor", 0.f);
			material.roughness = i["pbrMetallicRoughness"].value("roughnessFactor", 0.5f);
		}

		if (i["normalTexture"].is_object()) {
			unsigned int texIndex = i["normalTexture"]["index"];
			material.normalTex = m_Textures[texIndex];
			material.hasNormalTex = true;
		}

		materials.emplace_back(material);
	}

	return materials;
}

std::vector<TextureFile> GLTFLoader::getTextures()
{
	std::vector<TextureFile> textures;
	textures.reserve(JSON["textures"].size());

	std::string fileStr = std::string(m_File);
	std::string fileDir = fileStr.substr(0, fileStr.find_last_of('/') + 1);

	for (auto i : JSON["textures"])
	{
		unsigned int image = i["source"];
		std::string texPath = JSON["images"][image]["uri"];

		unsigned int sampler = i["sampler"];
		unsigned int mag = JSON["samplers"][sampler].value("magFilter", 0x2601); //GL_LINEAR
		unsigned int min = JSON["samplers"][sampler].value("minFilter", 0X2703); //GL_LINEAR_MIPMAP_LINEAR
		unsigned int wrapS = JSON["samplers"][sampler].value("wrapS", 0x2901); //GL_REPEAT
		unsigned int wrapT = JSON["samplers"][sampler].value("wrapT", 0x2901); //GL_REPEAT
		textures.emplace_back((fileDir + texPath), mag, min, wrapS, wrapT);
	}

	return textures;
}

std::vector<Vertex> GLTFLoader::groupVertices
(
	const std::vector<glm::vec3>& positions,
	const std::vector<glm::vec3>& normals,
	const std::vector<glm::vec2>& UVs
)
{
	std::vector<Vertex> vertices;
	vertices.reserve(positions.size());
	for (unsigned int i = 0; i < positions.size(); ++i)
	{
		vertices.emplace_back(
			positions[i],
			normals[i],
			UVs[i]
		);
	}

	return vertices;
}

std::vector<glm::vec2> GLTFLoader::groupVec2(const std::vector<float>& floatVec)
{
	std::vector<glm::vec2> vectors;
	vectors.reserve(floatVec.size());
	for (unsigned int i = 0; i < floatVec.size(); i += 2)
		vectors.emplace_back(glm::vec2(
			floatVec[i],
			floatVec[i + 1]
		));
	return vectors;
}

std::vector<glm::vec3> GLTFLoader::groupVec3(const std::vector<float>& floatVec)
{
	std::vector<glm::vec3> vectors;
	vectors.reserve(floatVec.size());
	//don't use i++ as the glm::vec3 constructor won't know in what order to read them
	for (unsigned int i = 0; i < floatVec.size(); i += 3)
		vectors.emplace_back(glm::vec3(
			floatVec[i],
			floatVec[i + 1],
			floatVec[i + 2]
		));

	return vectors;
}

std::vector<glm::vec4> GLTFLoader::groupVec4(const std::vector<float>& floatVec)
{
	std::vector<glm::vec4> vectors;
	vectors.reserve(floatVec.size());
	for (unsigned int i = 0; i < floatVec.size(); i += 4)
		vectors.emplace_back(glm::vec4(
			floatVec[i],
			floatVec[i + 1],
			floatVec[i + 2],
			floatVec[i + 3]
		));
	return vectors;
}

std::string GLTFLoader::getFileContents(const char* path)
{
	std::ifstream ifs(path);
	std::string text((std::istreambuf_iterator<char>(ifs)),
		(std::istreambuf_iterator<char>()));
	return text;
}