#pragma once
#include "Renderer.h"
#include "Test.h"
#include "FrameBuffer.h"
#include "Cubemap.h"
#include "Camera.h"

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

namespace test {
	class Test_PBR : public Test
	{
	public:
		Test_PBR();
		~Test_PBR();

		void OnUpdate(float deltaTime, GLFWwindow* window) override;
		void OnRender() override;
		void OnImGuiRender() override;

		void CursorInput(double xPos, double yPos) override;
		void ScrollInput(double xOffset, double yOffset) override;

		Camera cam;

	private:

		int cameraLock = 0;
		int inputFlag = 0;
		int mouseCtrl = 0;

		glm::vec3 lightPositions[4] = {
			glm::vec3(-10.0f,  10.0f, 10.0f),
			glm::vec3(10.0f,  10.0f, 10.0f),
			glm::vec3(-10.0f, -10.0f, 10.0f),
			glm::vec3(10.0f, -10.0f, 10.0f),
		};
		glm::vec3 lightColors[4] = {
			glm::vec3(300.0f, 300.0f, 300.0f),
			glm::vec3(300.0f, 300.0f, 300.0f),
			glm::vec3(300.0f, 300.0f, 300.0f),
			glm::vec3(300.0f, 300.0f, 300.0f)
		};

		int nrRows = 7;
		int nrColumns = 7;
		float spacing = 2.5;

		Shader PBRShader;

		unsigned int sphereVAO;
		unsigned int indexCount;

		float near = 0.1f;
		float far = 100.0f;

		//set projection matrices
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::perspective(glm::radians(cam.GetFov()), (float)WINDOW_WIDTH / WINDOW_HEIGHT, near, far);

		glm::mat4 mvp = glm::mat4(1.0f);

		Renderer renderer;
	};
}