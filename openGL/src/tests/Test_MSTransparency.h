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
	class Test_MSTransparency : public Test
	{
	public:
		Test_MSTransparency();
		~Test_MSTransparency();

		void OnUpdate(float deltaTime, GLFWwindow* window) override;
		void OnRender() override;
		void OnImGuiRender() override;

		void CursorInput(double xPos, double yPos) override;
		void ScrollInput(double xOffset, double yOffset) override;

		glm::vec3 pointLightPosition = glm::vec3(1.2f, 1.0f, 2.0f);

		Camera cam;

	private:

		int cameraLock = 0;
		int inputFlag = 0;
		int mouseCtrl = 0;

		float vertices[288] = {
			//pos				 //normals				//uv
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,	0.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,	1.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,	1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,	1.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,	0.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,	0.0f, 1.0f,

			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,	0.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,	1.0f,	1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,	1.0f,	1.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,	1.0f,	1.0f, 1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,	1.0f,	0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,	1.0f,	0.0f, 0.0f,

			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,	1.0f, 0.0f,
			-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,	1.0f, 1.0f,
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,	0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,	0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,	0.0f, 0.0f,
			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,	1.0f, 0.0f,

			 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,	1.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,	0.0f, 1.0f,
			 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,	1.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,	0.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,	1.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,	0.0f, 0.0f,

			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,	0.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,	1.0f, 1.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,	1.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,	1.0f, 0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,	0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,	0.0f, 1.0f,

			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,	0.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,	1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,	1.0f, 1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,	0.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,	1.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 	0.0f, 1.0f
		};

		glm::vec3 cubepositions[5] = {
			glm::vec3(3.0f, 0.0f, 0.0f),
			glm::vec3(1.5f, 0.0f, 0.0f),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(-1.5f, 0.0f, 0.0f),
			glm::vec3(-3.0f, 0.0f, 0.0f)
		};

		glm::vec3 pointLightPositions[4] = {
			glm::vec3(0.7f,  0.2f,  2.0f),
			glm::vec3(2.3f, -3.3f, -4.0f),
			glm::vec3(-4.0f,  2.0f, -12.0f),
			glm::vec3(0.0f,  0.0f, -3.0f)
		};

		//for framebuffers
		float quadVerts[24] = {
			// positions   // texCoords
			-1.0f,  1.0f,  0.0f, 1.0f,
			-1.0f, -1.0f,  0.0f, 0.0f,
			 1.0f, -1.0f,  1.0f, 0.0f,

			-1.0f,  1.0f,  0.0f, 1.0f,
			 1.0f, -1.0f,  1.0f, 0.0f,
			 1.0f,  1.0f,  1.0f, 1.0f
		};

		std::vector<std::string> skyboxFaces{
			"res/textures/Yokohama3/posx.jpg",
			"res/textures/Yokohama3/negx.jpg",
			"res/textures/Yokohama3/posy.jpg",
			"res/textures/Yokohama3/negy.jpg",
			"res/textures/Yokohama3/posz.jpg",
			"res/textures/Yokohama3/negz.jpg"
		};

		Shader shader;

		Shader outlineShader;
		Shader lightShader;
		
		Shader cubeMapShader;
		
		Shader compositeShader;
		Shader screenShader;
		Shader experimental;

		VertexArray va;
		VertexArray lightVA;
		VertexArray quadVA;

		FrameBuffer opaqueFB;
		unsigned int opaqueBuffer, depthBuffer;
		
		FrameBuffer transparentFB;
		unsigned int accumTexture, revealTexture;

		FrameBuffer opaqueScreenFB;
		unsigned int opaqueScreenTex;

		FrameBuffer transparentScreenFB;
		unsigned int accumScreenTex, revealScreenTex;

		glm::vec4 zeroFillerVec = glm::vec4(0.0f);
		glm::vec4 oneFillerVec = glm::vec4(1.0f);

		//set projection matrices
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);

		float near = 0.1f;
		float far = 100.0f;

		glm::mat4 normal = glm::mat4(1.0f);

		glm::mat4 mvp = glm::mat4(1.0f);
		glm::mat4 vp = glm::mat4(1.0f);

		Renderer renderer;

		const float radius = 5.0f;

		Cubemap skybox;
		
		glm::vec3 objectColor = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec3 pointLightColor = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec3 pointLightAmbient = glm::vec3(0.2f, 0.2f, 0.2f);
		glm::vec3 pointLightDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec3 pointLightSpecular = glm::vec3(1.0f, 1.0f, 1.0f);

		glm::vec3 dirLightColor = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec3 dirLightDirection = glm::vec3(-0.2f, -1.0f, -0.3f);
		glm::vec3 dirLightAmbient = glm::vec3(0.05f, 0.05f, 0.05f);
		glm::vec3 dirLightDiffuse = glm::vec3(0.4f, 0.4f, 0.4f);
		glm::vec3 dirLightSpecular = glm::vec3(0.0f, 0.0f, 0.0f);

		glm::vec3 matDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec3 matSpecular = glm::vec3(0.5f, 0.5f, 0.5f);
		float matShininess = 32.0f;

		glm::vec3 attenuationParams = glm::vec3(1.0f, 0.3f, 0.072f);

		int kuwahara_radius = 7;
	};
}