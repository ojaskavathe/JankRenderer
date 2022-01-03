#pragma once
#include <glm/glm.hpp>
#include "Model.h"
 
struct Object 
{
	Model model;
	glm::mat4 modelMat = glm::mat4(1.f);
	Shader* shader;
	 
	Object(Model model, Shader* shader)
		:model(model), shader(shader)
	{}
};

struct PointLight
{
	glm::vec3 position = glm::vec3(1.2f, 3.0f, 2.0f);
	glm::vec3 color = glm::vec3(10.f);

	//shadowmap
	unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
	float oAspect = (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT;
	float oNear = 0.1f;
	float oFar = 25.0f;
	glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), oAspect, oNear, oFar);


	PointLight(glm::vec3& pos, glm::vec3& color)
		:position(pos), color(color)
	{}
};

struct DirLight
{
	glm::vec3 direction = glm::vec3(-0.2f, -1.0f, -0.3f);
	glm::vec3 color = glm::vec3(1.f);

	DirLight(glm::vec3& dir, glm::vec3& color)
		:direction(dir), color(color)
	{}
};

class Scene
{
public:
	Scene() {}
	virtual ~Scene() {}

	void Add(Object& object);
	void Add(PointLight& light);
	void Add(DirLight& light);
	
	std::vector<Object*> m_Objects;
	std::vector<PointLight*> m_PointLights;
	std::vector<DirLight*> m_DirLights;
private:
};
