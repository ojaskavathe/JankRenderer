#pragma once

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include <string>
#include <vector>

#include "TextureFile.h"
#include "Renderer.h"
#include "VertexBufferLayout.h"
#include "Shader.h"

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoord;
};

struct Texture
{
	unsigned int id;
	std::string type;
	std::string path;
};

struct Material
{
	//sampler2D texture_diffuse1;
	glm::vec3 diffuse;
	//sampler2D texture_specular1;
	glm::vec3 specular;
	float shininess;
};

class Mesh
{
public:
	std::vector<Vertex> m_Vertices;
	std::vector<unsigned int> m_Indices;
	std::vector<Texture> m_Textures;
	//Material m_Material;
	unsigned int m_Blend;

	Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, std::vector<Texture>& textures, unsigned int &blend);
	//Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, Material &material, unsigned int &blend);
	void Draw(Shader &shader);

private:
	
	VertexArray vao;
	void SetupMesh();
};

