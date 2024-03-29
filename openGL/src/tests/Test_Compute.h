#pragma once
#include <vector>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include "loaders/GLTFLoader.h"

#include "Settings.h"

#include "Shader.h"
#include "VertexArray.h"
#include "FrameBuffer.h"
#include "Model.h"
#include "Camera.h"

#include "Test.h"

namespace test {
	class Test_Compute : public Test
	{
	public:
		Test_Compute();
		~Test_Compute();

		void OnUpdate(float deltaTime, GLFWwindow* window) override;
		void OnRender() override;
		void OnImGuiRender() override;

		void CursorInput(double xPos, double yPos) override;
		void ScrollInput(double xOffset, double yOffset) override;

		Camera cam = Camera(glm::vec3(0, 2, 5), glm::vec3(0, 1, 0), 270.f);

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

		Shader IBLShader;
		Shader lightShader;

		Shader depthMapShader;
		Shader omniDepthShader;

		Shader experimental;
		Shader compositeShader;
		Shader screenShader;

		Shader cubemapShader;

		VertexArray va;
		VertexArray lightVA;
		VertexArray quadVA;
		VertexArray planeVA;

		//shadowmap
		FrameBuffer depthMapFB;
		unsigned int depthMap;

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

		float near = 0.1f;
		float far = 100.0f;

		//glm::vec3 lightPosition = cam.GetCamPosition() + glm::vec3(0.0f, 4.0f, 0.0f);
		glm::vec3 lightPosition = glm::vec3(0.0f, 10.0f, 0.0f);

		//set projection matrices
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::perspective(glm::radians(cam.GetFov()), (float) WINDOW_WIDTH / WINDOW_HEIGHT, near, far);

		glm::mat4 mvp = glm::mat4(1.0f);
		glm::mat4 vp = glm::mat4(1.0f);

		glm::mat4 normal = glm::mat4(1.0f);

		//shadowmap matrices
		glm::mat4 lightProjection = glm::mat4(1.0f);
		glm::mat4 lightView = glm::mat4(1.0f);
		glm::mat4 lightVP = glm::mat4(1.0f);

		unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

		float shadowNear = 1.0f;
		float shadowFar = 15.f;

		int halfkernelWidth = 3;

		//Omnidirectional Shadowmap
		unsigned int oDepthMapFB; // <- Can't use FB class as we gotta use texture not texture2D for cubemap
		unsigned int depthCubemap;

		float oAspect = (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT;
		float oNear = 0.1f;
		float oFar = 25.0f;
		glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), oAspect, oNear, oFar);

		glm::vec4 clearColor = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);

		unsigned int lightPosSSBO;
		std::vector<glm::vec4> lightPos;

		glm::vec3 pointLightColor = glm::vec3(10.f);
		glm::vec3 pointLightAmbient = glm::vec3(0.2f);
		glm::vec3 pointLightDiffuse = glm::vec3(1.0f);
		glm::vec3 pointLightSpecular = glm::vec3(0.3f);

		glm::vec3 dirLightColor = glm::vec3(1.f);
		glm::vec3 dirLightDirection = glm::vec3(-0.2f, -1.0f, -0.3f);
		glm::vec3 dirLightAmbient = glm::vec3(0.1f);
		glm::vec3 dirLightDiffuse = glm::vec3(0.4f);
		glm::vec3 dirLightSpecular = glm::vec3(0.4f);

		glm::vec3 matDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec3 matSpecular = glm::vec3(0.5f, 0.5f, 0.5f);
		float matShininess = 32.0f;

		glm::vec3 attenuationParams = glm::vec3(1.0f, 0.3f, 0.44f);

		glm::vec4 color = glm::vec4(0.5f, 0.0f, 0.0f, 1.0f);

		float exposure = 0.4f;

		float metallic = 0.5f;
		float roughness = 0.1f;

		unsigned int hdrTex;
		unsigned int envCubemap;
		unsigned int irradianceMap;
		unsigned int prefilterMap;
		unsigned int brdfLUT;

		unsigned int comp;

		int swtch = 0;
		float lod = 0.f;
		int mapped = 0;
		float iblIntensity = 0.6f;

		Model mdl = Load::GLTF("res/models/JadeToad/JadeToad.gltf");
	};
}