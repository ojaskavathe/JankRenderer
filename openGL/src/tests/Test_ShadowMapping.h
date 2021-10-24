#pragma once
#include "Renderer.h"
#include "Test.h"
#include "FrameBuffer.h"
#include "Cubemap.h"
#include "Camera.h"
#include "TextureFile.h"

#include "Debug.h"

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

namespace test {
	class Test_ShadowMapping : public Test
	{
	public:
		Test_ShadowMapping();
		~Test_ShadowMapping();

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

		float vertices[288] = {
			//pos				 //normals				//uv
			-1.0f, -1.0, -1.0f,  0.0f,  0.0f, -1.0f,	0.0f, 0.0f,
			 1.0f,  1.0, -1.0f,  0.0f,  0.0f, -1.0f,	1.0f, 1.0f,
			 1.0f, -1.0, -1.0f,  0.0f,  0.0f, -1.0f,	1.0f, 0.0f,
			 1.0f,  1.0, -1.0f,  0.0f,  0.0f, -1.0f,	1.0f, 1.0f,
			-1.0f, -1.0, -1.0f,  0.0f,  0.0f, -1.0f,	0.0f, 0.0f,
			-1.0f,  1.0, -1.0f,  0.0f,  0.0f, -1.0f,	0.0f, 1.0f,
			 
			-1.0f, -1.0,  1.0f,  0.0f,  0.0f,  1.0f,	0.0f, 0.0f,
			 1.0f, -1.0,  1.0f,  0.0f,  0.0f,	1.0f,	1.0f, 0.0f,
			 1.0f,  1.0,  1.0f,  0.0f,  0.0f,	1.0f,	1.0f, 1.0f,
			 1.0f,  1.0,  1.0f,  0.0f,  0.0f,	1.0f,	1.0f, 1.0f,
			-1.0f,  1.0,  1.0f,  0.0f,  0.0f,	1.0f,	0.0f, 1.0f,
			-1.0f, -1.0,  1.0f,  0.0f,  0.0f,	1.0f,	0.0f, 0.0f,
			 
			-1.0f,  1.0,  1.0f, -1.0f,  0.0f,  0.0f,	1.0f, 0.0f,
			-1.0f,  1.0, -1.0f, -1.0f,  0.0f,  0.0f,	1.0f, 1.0f,
			-1.0f, -1.0, -1.0f, -1.0f,  0.0f,  0.0f,	0.0f, 1.0f,
			-1.0f, -1.0, -1.0f, -1.0f,  0.0f,  0.0f,	0.0f, 1.0f,
			-1.0f, -1.0,  1.0f, -1.0f,  0.0f,  0.0f,	0.0f, 0.0f,
			-1.0f,  1.0,  1.0f, -1.0f,  0.0f,  0.0f,	1.0f, 0.0f,
			 
			 1.0f,  1.0,  1.0f,  1.0f,  0.0f,  0.0f,	1.0f, 0.0f,
			 1.0f, -1.0, -1.0f,  1.0f,  0.0f,  0.0f,	0.0f, 1.0f,
			 1.0f,  1.0, -1.0f,  1.0f,  0.0f,  0.0f,	1.0f, 1.0f,
			 1.0f, -1.0, -1.0f,  1.0f,  0.0f,  0.0f,	0.0f, 1.0f,
			 1.0f,  1.0,  1.0f,  1.0f,  0.0f,  0.0f,	1.0f, 0.0f,
			 1.0f, -1.0,  1.0f,  1.0f,  0.0f,  0.0f,	0.0f, 0.0f,
			
			-1.0f, -1.0, -1.0f,  0.0f, -1.0f,  0.0f,	0.0f, 1.0f,
			 1.0f, -1.0, -1.0f,  0.0f, -1.0f,  0.0f,	1.0f, 1.0f,
			 1.0f, -1.0,  1.0f,  0.0f, -1.0f,  0.0f,	1.0f, 0.0f,
			 1.0f, -1.0,  1.0f,  0.0f, -1.0f,  0.0f,	1.0f, 0.0f,
			-1.0f, -1.0,  1.0f,  0.0f, -1.0f,  0.0f,	0.0f, 0.0f,
			-1.0f, -1.0, -1.0f,  0.0f, -1.0f,  0.0f,	0.0f, 1.0f,
			
			-1.0f,  1.0, -1.0f,  0.0f,  1.0f,  0.0f,	0.0f, 1.0f,
			 1.0f,  1.0,  1.0f,  0.0f,  1.0f,  0.0f,	1.0f, 0.0f,
			 1.0f,  1.0, -1.0f,  0.0f,  1.0f,  0.0f,	1.0f, 1.0f,
			-1.0f,  1.0,  1.0f,  0.0f,  1.0f,  0.0f,	0.0f, 0.0f,
			 1.0f,  1.0,  1.0f,  0.0f,  1.0f,  0.0f,	1.0f, 0.0f,
			-1.0f,  1.0, -1.0f,  0.0f,  1.0f,  0.0f, 	0.0f, 1.0f
		};

		glm::vec3 cubepositions[5] = {
			glm::vec3(3.0f, 0.0f, 0.0f),
			glm::vec3(1.5f, 0.0f, 0.0f),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(-1.5f, 0.0f, 0.0f),
			glm::vec3(-3.0f, 0.0f, 0.0f)
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

		float planeVerts[48] = {
			// positions            // normals         // texcoords
			25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
		   -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
		   -25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,

		   -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
			25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
			25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f
		};

		Shader shader;
		Shader lightShader;
		Shader screenShader;
		Shader depthMapShader;
		Shader basicShader;

		VertexArray va;
		VertexArray lightVA;
		VertexArray quadVA;
		VertexArray planeVA;

		FrameBuffer depthMapFB;
		unsigned int depthMap;
		unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

		float near = 0.1f;
		float far = 100.0f;

		float shadowNear = 1.0f;
		float shadowFar = 7.5f;

		int halfkernelWidth = 3;

		//glm::vec3 lightPosition = cam.GetCamPosition() + glm::vec3(0.0f, 4.0f, 0.0f);
		glm::vec3 lightPosition = glm::vec3(0.0f, 5.0f, 0.0f);

		//set projection matrices
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);

		glm::mat4 mvp = glm::mat4(1.0f);
		glm::mat4 vp = glm::mat4(1.0f);

		glm::mat4 normal = glm::mat4(1.0f);

		//shadowmap matrices
		glm::mat4 lightProjection = glm::mat4(1.0f);
		glm::mat4 lightView = glm::mat4(1.0f);
		glm::mat4 lightVP = glm::mat4(1.0f);

		Renderer renderer;
		
		glm::vec3 pointLightPosition = glm::vec3(1.2f, 1.0f, 2.0f);
		
		glm::vec3 objectColor = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec3 pointLightColor = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec3 pointLightAmbient = glm::vec3(0.2f);
		glm::vec3 pointLightDiffuse = glm::vec3(1.0f);
		glm::vec3 pointLightSpecular = glm::vec3(1.0f);

		glm::vec3 dirLightColor = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec3 dirLightDirection = glm::vec3(-0.2f, -1.0f, -0.3f);
		glm::vec3 dirLightAmbient = glm::vec3(0.5f);
		glm::vec3 dirLightDiffuse = glm::vec3(0.4f);
		glm::vec3 dirLightSpecular = glm::vec3(0.4f);

		glm::vec3 matDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec3 matSpecular = glm::vec3(0.5f, 0.5f, 0.5f);
		float matShininess = 32.0f;

		glm::vec3 attenuationParams = glm::vec3(1.0f, 0.3f, 0.44f);

		glm::vec4 color = glm::vec4(0.5f, 0.0f, 0.0f, 1.0f);
	};
}