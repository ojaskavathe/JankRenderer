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
	//sampler2D texture_specular1;
	glm::vec4 albedo;
	float metallic;
	float roughness;
};

class Mesh
{
public:
	std::vector<Vertex> m_Vertices;
	std::vector<unsigned int> m_Indices;
	std::vector<Texture> m_Textures;
	std::vector<Material> m_Materials;

	Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, std::vector<Material> &materials);
	void Draw(Shader &shader, glm::mat4 model, glm::mat4& vp);

private:
	
	VertexArray vao;
	void SetupMesh();
};

