#pragma once
#include <glm/glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include "VertexBufferLayout.h"
#include "Renderer.h"
#include "Camera.h"

namespace Debug {

	static glm::mat4 vp;

	void SetViewProj(glm::mat4& viewProj);
	void DrawLine(glm::vec3 start, glm::vec3 end);
	void DrawTri(glm::vec3 one, glm::vec3 two, glm::vec3 three);
	void DrawOrthProj(glm::mat4& viewProj);
}