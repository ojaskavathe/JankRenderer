#pragma once

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include <string>
#include <vector>

#include "TextureFile.h"
#include "VertexArray.h"
#include "Shader.h"
#include "IndexBuffer.h"

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoord;

	glm::vec3 tangent;
	glm::vec3 bitangent;

	Vertex(const glm::vec3& pos, const glm::vec3& norm, const glm::vec2& uv) 
		: position(pos), normal(norm), texCoord(uv), tangent(glm::vec3(0)), bitangent(glm::vec3(0))
	{
	}
};

struct Triangle
{
	Vertex &v1;
	Vertex &v2;
	Vertex &v3;

	Triangle(Vertex* first, Vertex* second, Vertex* third)
		:v1(*first), v2(*second), v3(*third)
	{
	}
};

struct Material
{
	glm::vec4 albedo = glm::vec4(0.7f, 0.7f, 0.7f, 1.f);
	float metallic = 0.f;
	float roughness = 0.1f;

	bool hasAlbedoTex = false;
	bool hasMetRoughTex = false;
	bool hasNormalTex = false;

	TextureFile albedoTex;
	TextureFile metallicRoughnessTex;
	TextureFile normalTex;
};

struct Primitive
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	Material material;

	std::vector<Triangle> Triangles;

	//VertexBuffer vbo;
	//IndexBuffer ibo;
	VertexArray vao;

	Primitive(std::vector<Vertex> vertices, std::vector<unsigned int> indices, Material mat)
		: vertices(vertices), indices(indices), material(mat)
	{
	}
};

class Mesh
{
	std::vector<Primitive> m_Primitives;

	void SetupMesh();
	void SetupTris(Primitive& prim);
	void InitTangentBasis(Primitive& prim);

public:
	Mesh(std::vector<Primitive> primitives);
	const void Draw(const Shader& shader, const glm::mat4& model, const glm::mat4& vp);
	const void DrawShadowMap(const Shader& shader, const glm::mat4& model, const glm::mat4& vp);
};