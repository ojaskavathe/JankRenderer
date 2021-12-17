#include "Model.h"

Model::Model(const char* path)
{
	std::string text = getFileContents(path);
	JSON = json::parse(text);

	Model::file = path;
	data = getData();
}

void Model::Draw(Shader& shader, glm::vec3 camDist)
{
	
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

std::vector<Vertex> Model::groupVertices
(
	std::vector<glm::vec3> positions,
	std::vector<glm::vec3> normals,
	std::vector<glm::vec2> UVs
)
{
	std::vector<Vertex> vertices;
	for (unsigned int i; i < positions.size(); ++i)
	{
		vertices.push_back(
			Vertex{
				positions[i],
				normals[i],
				UVs[i]
			}
		);
	}
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

//void Model::LoadModel(const std::string& path)
//{
//	Assimp::Importer importer;
//
//	//aiProcess_FlipUVs doesn't quite work for everything
//	const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate); 
//
//	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
//	{
//		std::cout << "[AssImp]: " << importer.GetErrorString() << std::endl;
//		return;
//	}
//
//	directory = path.substr(0, path.find_last_of('/'));
//	ProcessNode(scene->mRootNode, scene);
//
//	std::cout << "Meshes: " << scene->mNumMeshes << std::endl;
//	std::cout << "Materials: " << scene->mNumMaterials << std::endl;
//
//	for (int i = 0; i < meshes.size(); i++)
//	{
//		if (meshes[i].m_Blend == 0) opaqueMeshes.push_back(meshes[i]);
//		else if (meshes[i].m_Blend != 0) transparentMeshes.push_back(meshes[i]);
//	}
//
//	std::cout << "Opaque Meshes: " << opaqueMeshes.size() << std::endl;
//	std::cout << "Transparent Meshes: " << transparentMeshes.size() << std::endl;
//}
//
//void Model::ProcessNode(aiNode * node, const aiScene * scene)
//{
//	for (unsigned int i = 0; i < node->mNumMeshes; i++)
//	{
//		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
//		meshes.push_back(ProcessMesh(mesh, scene));
//		numFaces.push_back(mesh->mNumFaces);
//	}
//	//recursive
//	for (unsigned int i = 0; i < node->mNumChildren; i++)
//	{
//		ProcessNode(node->mChildren[i], scene);
//	}
//}
//
//Mesh Model::ProcessMesh(aiMesh * mesh, const aiScene * scene)
//{
//	std::vector<Vertex> vertices;
//	std::vector<unsigned int> indices;
//	std::vector<Texture> textures;
//	unsigned int blend;
//	//retrieve vertices
//	//3 parts process -> positions, normals, texCoords
//	for (unsigned int i = 0; i < mesh->mNumVertices; i++) 
//	{
//		Vertex vertex;
//
//		//pos
//		//(note to self: assimp calls vertex positions mVertices -_-)
//		glm::vec3 vec;
//		vec.x = mesh->mVertices[i].x;
//		vec.y = mesh->mVertices[i].y;
//		vec.z = mesh->mVertices[i].z;
//		vertex.position = vec;
//
//		//normals
//		if (mesh->HasNormals())
//		{
//			vec.x = mesh->mNormals[i].x;
//			vec.y = mesh->mNormals[i].y;
//			vec.z = mesh->mNormals[i].z;
//			vertex.normal = vec;	
//		}
//		
//		//texCoords
//		if (mesh->mTextureCoords[0]) 
//		{
//			glm::vec2 vec2;
//			vec2.x = mesh->mTextureCoords[0][i].x;
//			vec2.y = mesh->mTextureCoords[0][i].y;
//			vertex.texCoord = vec2;
//		}
//		else
//			vertex.texCoord = glm::vec2(0.0f, 0.0f);
//
//		vertices.push_back(vertex);
//	}
//
//	//retrieve indices
//	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
//	{
//		aiFace face = mesh->mFaces[i];
//		/*for (unsigned int j = 0; j < face.mNumIndices; j++)
//		{
//			indices.push_back(face.mIndices[j]);
//		}*/
//		glm::vec3 pos[3];
//
//		if (face.mNumIndices == 3) {
//			indices.push_back(face.mIndices[0]);
//			indices.push_back(face.mIndices[1]);
//			indices.push_back(face.mIndices[2]);
//
//			pos[0] = vertices[face.mIndices[0]].position;
//			pos[1] = vertices[face.mIndices[1]].position;
//			pos[2] = vertices[face.mIndices[2]].position;
//			facePos.push_back((pos[0] + pos[1] + pos[2]) / glm::float32(3.0f));
//			
//			//for all the indices in each face, add each indice
//		}
//	}
//
//	//retrieve materials
//	if (mesh->mMaterialIndex >= 0)
//	{
//		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
//		std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
//		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
//		std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
//		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
//
//		material->Get(AI_MATKEY_BLEND_FUNC, blend);
//	}
//
//	
//	return Mesh(vertices, indices, textures, blend);
//}
//
//std::vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName)
//{
//	std::vector<Texture> textures;
//
//	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
//	{
//		aiString str;
//		mat->GetTexture(type, i, &str);
//		bool loaded = false;
//		
//		//check if the texture's already loaded
//		for (unsigned int j = 0; j < textures_loaded.size(); j++)
//		{
//			if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
//			{
//				loaded = true;
//				textures.push_back(textures_loaded[j]);
//				break;
//			}
//		}
//
//		if (loaded == false)
//		{
//			Texture texture;
//			texture.id = TextureFile(directory + "/" + std::string(str.C_Str())).GetId();
//			texture.type = typeName;
//			texture.path = str.C_Str();
//			textures.push_back(texture);
//			//add to loaded textures
//			textures_loaded.push_back(texture);
//		}
//	}
//	return textures;
//}