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

	glm::vec3 tangent;
	glm::vec3 bitangent;
};

struct Triangle
{
	Vertex &v1;
	Vertex &v2;
	Vertex &v3;
};

struct Material
{
	glm::vec4 albedo;
	float metallic;
	float roughness;

	bool hasAlbedoTex = false;
	bool hasMetRoughTex = false;
	bool hasNormalTex = false;

	TextureFile albedoTex;
	TextureFile metallicRoughnessTex;
	TextureFile normalTex;

	/*Material(glm::vec4 albedo, float metallic, float roughness)
		: albedo(albedo), metallic(metallic), roughness(roughness)
	{};*/
};

struct Primitive
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	Material material;

	std::vector<Triangle> Triangles;

	VertexArray vao;
};

class Mesh
{
	std::vector<Primitive> m_Primitives;

	void SetupMesh();
	void SetupTris(Primitive& prim);
	void InitTangentBasis(Primitive& prim);

public:
	Mesh(std::vector<Primitive> primitives);
	void Draw(Shader& shader, glm::mat4& model, glm::mat4& vp);
};