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
		material.roughness = 0.1f;

		cube.push_back(Primitive{ vertices, indices, material });

		std::vector<Mesh> meshes;
		meshes.reserve(1);
		meshes.emplace_back(Mesh(cube));
		return Model(meshes);
	}

	Model Sphere()
	{
		return Model("res/models/sphere/sphere.gltf");
	}
}