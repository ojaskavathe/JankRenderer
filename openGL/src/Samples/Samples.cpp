#include <iostream>
#include "Samples.h"

namespace Samples {
	Model Cube()
	{
		std::vector<Primitive> cube;
		cube.reserve(1);

		std::vector<Vertex> vertices = {
					Vertex(glm::vec3(-1, -1,  1), glm::vec3(-1,  0, -0), glm::vec2(0.375, 1)),
					Vertex(glm::vec3(-1, -1,  1), glm::vec3(0, -1, -0),	 glm::vec2(0.125, 0.25)),
					Vertex(glm::vec3(-1, -1,  1), glm::vec3(0,  0,  1),	 glm::vec2(0.375, 0)),
					Vertex(glm::vec3(-1,  1,  1), glm::vec3(-1,  0, -0), glm::vec2(0.625, 1)),
					Vertex(glm::vec3(-1,  1,  1), glm::vec3(0,  0,  1),	 glm::vec2(0.625, 0)),
					Vertex(glm::vec3(-1,  1,  1), glm::vec3(0,  1, -0),	 glm::vec2(0.875, 0.25)),
					Vertex(glm::vec3(-1, -1, -1), glm::vec3(-1,  0, -0), glm::vec2(0.375, 0.75)),
					Vertex(glm::vec3(-1, -1, -1), glm::vec3(0, -1, -0),	 glm::vec2(0.125, 0.5)),
					Vertex(glm::vec3(-1, -1, -1), glm::vec3(0,  0, -1),	 glm::vec2(0.375, 0.75)),
					Vertex(glm::vec3(-1,  1, -1), glm::vec3(-1,  0, -0), glm::vec2(0.625, 0.75)),
					Vertex(glm::vec3(-1,  1, -1), glm::vec3(0,  0, -1),	 glm::vec2(0.625, 0.75)),
					Vertex(glm::vec3(-1,  1, -1), glm::vec3(0,  1, -0),	 glm::vec2(0.875, 0.5)),
					Vertex(glm::vec3(1, -1,  1),  glm::vec3(0, -1, -0),	 glm::vec2(0.375, 0.25)),
					Vertex(glm::vec3(1, -1,  1),  glm::vec3(0,  0,  1),	 glm::vec2(0.375, 0.25)),
					Vertex(glm::vec3(1, -1,  1),  glm::vec3(1,  0, -0),	 glm::vec2(0.375, 0.25)),
					Vertex(glm::vec3(1,  1,  1),  glm::vec3(0,  0,  1),	 glm::vec2(0.625, 0.25)),
					Vertex(glm::vec3(1,  1,  1),  glm::vec3(0,  1, -0),	 glm::vec2(0.625, 0.25)),
					Vertex(glm::vec3(1,  1,  1),  glm::vec3(1,  0, -0),	 glm::vec2(0.625, 0.25)),
					Vertex(glm::vec3(1, -1, -1),  glm::vec3(0, -1, -0),	 glm::vec2(0.375, 0.5)),
					Vertex(glm::vec3(1, -1, -1),  glm::vec3(0,  0, -1),	 glm::vec2(0.375, 0.5)),
					Vertex(glm::vec3(1, -1, -1),  glm::vec3(1,  0, -0),	 glm::vec2(0.375, 0.5)),
					Vertex(glm::vec3(1,  1, -1),  glm::vec3(0,  0, -1),	 glm::vec2(0.625, 0.5)),
					Vertex(glm::vec3(1,  1, -1),  glm::vec3(0,  1, -0),	 glm::vec2(0.625, 0.5)),
					Vertex(glm::vec3(1,  1, -1),  glm::vec3(1,  0, -0),	 glm::vec2(0.625, 0.5)),
		};

		std::vector<unsigned int> indices = {
					0, 3, 9,
					0, 9, 6,
					8, 10, 21,
					8, 21, 19,
					20, 23, 17,
					20, 17, 14,
					13, 15, 4,
					13, 4, 2,
					7, 18, 12,
					7, 12, 1,
					22, 11, 5,
					22, 5, 16,
		};

		Material material;
		material.albedo = glm::vec4(0.7f, 0.7f, 0.7f, 1.f);
		material.metallic = 0.f;
		material.roughness = 0.5f;

		cube.push_back(Primitive{ vertices, indices, material });

		std::vector<Mesh> meshes;
		meshes.reserve(1);
		meshes.emplace_back(Mesh(cube));
		return Model(meshes);
	}

	Mesh Sphere()
	{
		std::vector<Primitive> sphere;
		sphere.reserve(1);

		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;

		const unsigned int X_SEGMENTS = 64;
		const unsigned int Y_SEGMENTS = 64;
		const float PI = 3.14159265359;
		for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
		{
			for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
			{
				float xSegment = (float)x / (float)X_SEGMENTS;
				float ySegment = (float)y / (float)Y_SEGMENTS;
				float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
				float yPos = std::cos(ySegment * PI);
				float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

				vertices.push_back(
					Vertex(
						glm::vec3(xPos, yPos, zPos),
						glm::vec3(xPos, yPos, zPos),
						glm::vec2(xSegment, ySegment)
					)
				);
			}
		}

		bool oddRow = false;
		for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
		{
			if (!oddRow) // even rows: y == 0, y == 2; and so on
			{
				for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
				{
					indices.push_back(y * (X_SEGMENTS + 1) + x);
					indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
				}
			}
			else
			{
				for (int x = X_SEGMENTS; x >= 0; --x)
				{
					indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
					indices.push_back(y * (X_SEGMENTS + 1) + x);
				}
			}
			oddRow = !oddRow;
		}

		Material material;
		material.albedo = glm::vec4(0.7f, 0.7f, 0.7f, 1.f);
		material.metallic = 0.f;
		material.roughness = 0.5f;

		sphere.push_back(Primitive{ vertices, indices, material });

		return Mesh(sphere);
	}
}