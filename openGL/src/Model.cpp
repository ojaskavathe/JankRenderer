#include "Model.h"

Model::Model(const std::string& path)
{
	LoadModel(path);
}

void Model::Draw(Shader& shader, glm::vec3 camDist)
{
	////std::map<float, Mesh> sorted;
	//std::vector<float> dist;
	////if you ever come back to this, find a way to order meshes and THEN order their faces.
	//for (unsigned int i = 0; i < meshes.size(); i++)
	//	for (unsigned int j = 0; j < numFaces[i]; j++)
	//		dist.push_back(glm::length(camDist - facePos[std::accumulate(numFaces.begin(), numFaces.begin() + i, 0)+j]));

	//unsigned int i, j, min_idx;
	//// One by one move boundary of unsorted subarray
	//for (unsigned int k = 0; k < meshes.size(); k++)
	//{
	//	for (i = 0; i < numFaces[k] - 1; i++)
	//	{
	//		// Find the minimum dist
	//		min_idx = i;
	//		for (j = i + 1; j < numFaces[k]; j++)
	//			if (dist[(std::accumulate(numFaces.begin(), numFaces.begin() + k, 0))+j] > dist[(std::accumulate(numFaces.begin(), numFaces.begin() + k, 0)) + min_idx])
	//				min_idx = j;

	//		// Swap the found minimum dist
	//		float temp = dist[min_idx];
	//		dist[min_idx] = dist[i];
	//		dist[i] = temp;

	//		unsigned int tempIndex = meshes[k].m_Indices[min_idx];
	//		meshes[k].m_Indices[min_idx] = meshes[0].m_Indices[i];
	//		meshes[k].m_Indices[i] = tempIndex;
	//	}
	//}

	//NOTHING WORKS FOR THE LOVE OF GOD FIGURE OUT ORDER INDEPENDENCE
	//UPDATE(DAY 4): can confirm it still doesn't work askdhbaksdhbasdgkk
	//UPDATE(DAY 7): why u think it'll magically start working when you come back

	////for (std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
	//{
	//	//meshes[i].Draw(shader);
	//}
	
	/*glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
	for (unsigned int i = 0; i < opaqueMeshes.size(); i++)
		opaqueMeshes[i].Draw(shader);

	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	for (unsigned int i = 0; i < transparentMeshes.size(); i++)
		transparentMeshes[i].Draw(shader);*/

	for (int i = 0; i < meshes.size(); i++)
		meshes[i].Draw(shader);
}

void Model::LoadModel(const std::string& path)
{
	Assimp::Importer importer;

	//aiProcess_FlipUVs doesn't quite work for everything
	const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate); 

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
	{
		std::cout << "[AssImp]: " << importer.GetErrorString() << std::endl;
		return;
	}

	directory = path.substr(0, path.find_last_of('/'));
	ProcessNode(scene->mRootNode, scene);

	std::cout << "Meshes: " << scene->mNumMeshes << std::endl;
	std::cout << "Materials: " << scene->mNumMaterials << std::endl;

	for (int i = 0; i < meshes.size(); i++)
	{
		if (meshes[i].m_Blend == 0) opaqueMeshes.push_back(meshes[i]);
		else if (meshes[i].m_Blend != 0) transparentMeshes.push_back(meshes[i]);
	}

	std::cout << "Opaque Meshes: " << opaqueMeshes.size() << std::endl;
	std::cout << "Transparent Meshes: " << transparentMeshes.size() << std::endl;
}

void Model::ProcessNode(aiNode * node, const aiScene * scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(ProcessMesh(mesh, scene));
		numFaces.push_back(mesh->mNumFaces);
	}
	//recursive
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene);
	}
}

Mesh Model::ProcessMesh(aiMesh * mesh, const aiScene * scene)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
	unsigned int blend;
	//retrieve vertices
	//3 parts process -> positions, normals, texCoords
	for (unsigned int i = 0; i < mesh->mNumVertices; i++) 
	{
		Vertex vertex;

		//pos
		//(note to self: assimp calls vertex positions mVertices -_-)
		glm::vec3 vec;
		vec.x = mesh->mVertices[i].x;
		vec.y = mesh->mVertices[i].y;
		vec.z = mesh->mVertices[i].z;
		vertex.position = vec;

		//normals
		if (mesh->HasNormals())
		{
			vec.x = mesh->mNormals[i].x;
			vec.y = mesh->mNormals[i].y;
			vec.z = mesh->mNormals[i].z;
			vertex.normal = vec;	
		}
		
		//texCoords
		if (mesh->mTextureCoords[0]) 
		{
			glm::vec2 vec2;
			vec2.x = mesh->mTextureCoords[0][i].x;
			vec2.y = mesh->mTextureCoords[0][i].y;
			vertex.texCoord = vec2;
		}
		else
			vertex.texCoord = glm::vec2(0.0f, 0.0f);

		vertices.push_back(vertex);
	}

	//retrieve indices
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		/*for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}*/
		glm::vec3 pos[3];

		if (face.mNumIndices == 3) {
			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);

			pos[0] = vertices[face.mIndices[0]].position;
			pos[1] = vertices[face.mIndices[1]].position;
			pos[2] = vertices[face.mIndices[2]].position;
			facePos.push_back((pos[0] + pos[1] + pos[2]) / glm::float32(3.0f));
			
			//for all the indices in each face, add each indice
		}
	}

	//retrieve materials
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

		material->Get(AI_MATKEY_BLEND_FUNC, blend);
	}

	/*Material material;
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
		aiColor3D color(0.f, 0.f, 0.f);
		float shininess;

		mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
		material.diffuse = glm::vec3(color.r, color.b, color.g);

		mat->Get(AI_MATKEY_COLOR_SPECULAR, color);
		material.specular = glm::vec3(color.r, color.b, color.g);

		mat->Get(AI_MATKEY_SHININESS, shininess);
		material.shininess = shininess;

		mat->Get(AI_MATKEY_BLEND_FUNC, blend);
	}*/

	//firstVert.push_back(vertices[0]);
	return Mesh(vertices, indices, textures, blend);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName)
{
	std::vector<Texture> textures;

	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		bool loaded = false;
		
		//check if the texture's already loaded
		for (unsigned int j = 0; j < textures_loaded.size(); j++)
		{
			if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
			{
				loaded = true;
				textures.push_back(textures_loaded[j]);
				break;
			}
		}

		if (loaded == false)
		{
			Texture texture;
			texture.id = TextureFile(directory + "/" + std::string(str.C_Str())).GetId();
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			//add to loaded textures
			textures_loaded.push_back(texture);
		}
	}
	return textures;
}