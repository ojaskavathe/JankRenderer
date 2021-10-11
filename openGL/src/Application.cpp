#include <glad\glad.h>
#include <GLFW\glfw3.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include <iostream>

#include "Camera.h"

#include "Shader.h"
#include "TextureFile.h"

#include "Renderer.h"

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "FrameBuffer.h"

#include "Cubemap.h"

#include "Model.h"

#include "imgui\imgui.h"
#include "imgui\imgui_impl_opengl3.h"
#include "imgui\imgui_impl_glfw.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xPos, double yPos);
void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);

float deltaTime = 0.0f;
float LastFrame = 0.0f;

int cameraLock = 0;
int inputFlag = 0;
int mouseCtrl = 0;

Camera cam;

glm::vec3 pointLightPosition(1.2f, 1.0f, 2.0f);

int main()
{

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "AHAHAHAHA", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "GLAD failed to load" << std::endl;
		return -1;
	}

	//enable gl debugging
	SetDebugCallback();

	//init experimental
	Shader shader("res/shaders/shaderv.vert", "res/shaders/shaderf.frag");
	//Shader shader("res/shaders/basicv.vert", "res/shaders/basicf.frag");
	Shader frameBufferShader("res/shaders/frameBufferv.vert", "res/shaders/frameBufferf.frag");
	Shader outlineShader("res/shaders/basicv.vert", "res/shaders/stencilOutline.frag");

	Shader lightShader("res/shaders/lightShaderv.vert", "res/shaders/LightShaderf.frag");

	Shader cubeMapShader("res/shaders/cubemapv.vert", "res/shaders/cubemapf.frag");

	Shader solidShader("res/shaders/solidShaderv.vert", "res/shaders/solidShaderf.frag");
	Shader transparentShader("res/shaders/transparentShaderv.vert", "res/shaders/transparentShaderf.frag");
	Shader compositeShader("res/shaders/compositeShaderv.vert", "res/shaders/compositeShaderf.frag");
	Shader screenShader("res/shaders/screenShaderv.vert", "res/shaders/screenShaderf.frag");
	Shader experimental("res/shaders/shaderv.vert", "res/shaders/experimentalf.frag");

	std::vector<std::string> skyboxFaces{
		"res/textures/Yokohama3/posx.jpg",
		"res/textures/Yokohama3/negx.jpg",
		"res/textures/Yokohama3/posy.jpg",
		"res/textures/Yokohama3/negy.jpg",
		"res/textures/Yokohama3/posz.jpg",
		"res/textures/Yokohama3/negz.jpg"
	};
	
	float vertices[] = {
	//pos				 //normals				//uv
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,	0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,	1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,	1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,	1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,	0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,	0.0f, 0.0f,

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
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,	1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,	0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,	0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,	0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,	1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,	0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,	1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,	1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,	1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,	0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,	0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,	0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,	1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,	1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,	1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,	0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 	0.0f, 1.0f 
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

	glm::vec3 cubepositions[] = {
		glm::vec3(3.0f, 0.0f, 0.0f),
		glm::vec3(1.5f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(-1.5f, 0.0f, 0.0f),
		glm::vec3(-3.0f, 0.0f, 0.0f)
	};

	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.7f,  0.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
	};

	//init arrays and buffers
	VertexArray va;
	VertexBuffer vb(vertices, unsigned int(sizeof(vertices)));
	VertexBufferLayout layout;

	//position
	layout.Push<float>(3);

	//normals
	layout.Push<float>(3);

	//textCoords
	layout.Push<float>(2);
	va.AddBuffer(vb, layout);
	va.Unbind();

	//lightVertexArray
	VertexArray lightVA;
	VertexBufferLayout lightLayout;
	//setting light vertex attributes
	lightLayout.Push<float>(3);
	lightLayout.strideOverride(5, GL_FLOAT);
	lightVA.AddBuffer(vb, lightLayout);
	lightVA.Unbind();

	VertexArray quadVA;
	quadVA.Bind();
	VertexBuffer quadVB(quadVerts, unsigned int(sizeof(quadVerts)));
	VertexBufferLayout quadLayout;
	quadLayout.Push<float>(2);
	quadLayout.Push<float>(2);
	quadVA.AddBuffer(quadVB, quadLayout);
	quadVA.Unbind();

	//transparency framebuffers
	unsigned int opaqueFB, transparentFB;
	glGenFramebuffers(1, &opaqueFB);
	glGenFramebuffers(1, &transparentFB);

	unsigned int opaqueBuffer;
	glGenTextures(1, &opaqueBuffer);
	glBindTexture(GL_TEXTURE_2D, opaqueBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGBA, GL_HALF_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	unsigned int depthBuffer;
	glGenTextures(1, &depthBuffer);
	glBindTexture(GL_TEXTURE_2D, depthBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	//bind opaque attachments to framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, opaqueFB);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, opaqueBuffer, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthBuffer, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//transparent buffers
	unsigned int accumTexture, revealTexture;
	glGenTextures(1, &accumTexture);
	glBindTexture(GL_TEXTURE_2D, accumTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGBA, GL_HALF_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &revealTexture);
	glBindTexture(GL_TEXTURE_2D, revealTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RED, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	//bind transparent attachments
	glBindFramebuffer(GL_FRAMEBUFFER, transparentFB);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, accumTexture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, revealTexture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthBuffer, 0);

	//explicitly telling opengl we have 2 draw buffers for the transparent framebuffer
	const GLenum transparentDrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, transparentDrawBuffers);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glm::vec4 zeroFillerVec(0.0f);
	glm::vec4 oneFillerVec(1.0f);

	//set projection matrices
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection;

	float near = 0.1f;
	float far = 100.0f;

	glm::mat4 normal;

	glm::mat4 mvp;
	glm::mat4 vp;

	Renderer renderer;

	//imgui setup
	const char* glsl_version = "#version 460";
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	const float radius = 5.0f;

	//wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	Cubemap skybox;
	skybox.Load(skyboxFaces);

	stbi_set_flip_vertically_on_load(true);

	//Model subject("res/models/backpack/backpack.obj");
	//Model subject("res/models/sponza/sponza_flat.obj");
	Model subject("res/models/crt/crt_split.obj");
	//Model subject("res/models/plantCapsule/plantCapsule.obj");
	//Model subject("res/models/suzanne/suzanne.obj");

	glm::vec3 objectColor(1.0f, 1.0f, 1.0f);
	glm::vec3 pointLightColor	 (1.0f, 1.0f, 1.0f);
	glm::vec3 pointLightAmbient  (0.2f, 0.2f, 0.2f);
	glm::vec3 pointLightDiffuse  (1.0f, 1.0f, 1.0f);
	glm::vec3 pointLightSpecular (1.0f, 1.0f, 1.0f);

	glm::vec3 dirLightColor(1.0f, 1.0f, 1.0f);
	glm::vec3 dirLightDirection	  (-0.2f, -1.0f, -0.3f);
	glm::vec3 dirLightAmbient (0.05f,  0.05f, 0.05f);
	glm::vec3 dirLightDiffuse (0.4f,  0.4f,  0.4f);
	glm::vec3 dirLightSpecular(0.0f,  0.0f,  0.0f);

	glm::vec3 matDiffuse (1.0f, 1.0f, 1.0f);
	glm::vec3 matSpecular(0.5f, 0.5f, 0.5f);
	float matShininess = 32.0f;

	glm::vec3 attenuationParams(1.0f, 0.3f, 0.072f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	//backface culling
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);

	//blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	int kuwahara_radius = 7;

	while (!glfwWindowShouldClose(window))
	{
		//setDeltaTime
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - LastFrame;
		LastFrame = currentFrame;

		//input
		processInput(window);

		//renderer.Clear();
		projection = glm::perspective(glm::radians(cam.GetFov()), 800.0f / 600.0f, near, far);

		//Init imgui
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		//set depth testing for opaque rendering
		glEnable(GL_DEPTH_TEST); //<-- keep track
		glDepthFunc(GL_LESS);
		glDepthMask(GL_TRUE); //<-- start writing to depth buffer
		glDisable(GL_BLEND);
		glClearColor(0.3f, 0.2f, 0.3f, 1.0f);

		//bind opaque framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, opaqueFB);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		//opaque rendering begins
		
		//cubemap
		view = glm::mat4(glm::mat3(cam.GetViewMatrix())); //<-- remove translation due to camera

		vp = projection * view;
		cubeMapShader.Bind();
		cubeMapShader.SetUniformMatrix4fv("vp", vp);
		
		//skybox.Render(cubeMapShader);

		view = cam.GetViewMatrix(); //<-- add translation back to camera

		//render lights
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF); //<-- Start writing to stencil buffer

		lightShader.Bind();

		model = glm::mat4(1.0f);
		model = glm::translate(model, pointLightPosition);
		model = glm::scale(model, glm::vec3(0.2f));
		mvp = projection * view * model;
		lightShader.SetUniformMatrix4fv("mvp", mvp);
		lightShader.SetUniform3fv("lightColor", pointLightColor);

		renderer.DrawArrays(lightVA, vb, lightShader);

		//light stencil
		glStencilMask(0x00); //<-- stop writing to stencil buffer
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glDisable(GL_DEPTH_TEST);
		
		model = glm::scale(model, glm::vec3(1.1f));
		mvp = projection * view * model;
		outlineShader.Bind();
		outlineShader.SetUniformMatrix4fv("mvp", mvp);
		renderer.DrawArrays(lightVA, vb, outlineShader);
		
		glEnable(GL_DEPTH_TEST);
		glStencilMask(0x00);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);

		//render cubes
		model = glm::mat4(1.0f);
		glm::vec4 color;

		shader.Bind();

		shader.SetUniform1f("near", near);
		shader.SetUniform1f("far", far);

		shader.SetUniform3fv("viewPosition", cam.GetCamPosition());

		shader.SetUniform3fv("dirLight.color", dirLightColor);
		shader.SetUniform3fv("dirLight.direction", dirLightDirection);
		shader.SetUniform3fv("dirLight.ambient", dirLightAmbient);
		shader.SetUniform3fv("dirLight.diffuse", dirLightDiffuse);
		shader.SetUniform3fv("dirLight.specular", dirLightSpecular);

		shader.SetUniform3fv("pointLight.color", pointLightColor);
		shader.SetUniform3fv("pointLight.position", pointLightPosition);
		shader.SetUniform3fv("pointLight.ambient", pointLightAmbient);
		shader.SetUniform3fv("pointLight.diffuse", pointLightDiffuse);
		shader.SetUniform3fv("pointLight.specular", pointLightSpecular);
		shader.SetUniform3fv("pointLight.atten", attenuationParams);

		shader.SetUniform1f("mat.shininess", matShininess);

		model = glm::translate(model, cubepositions[2]);
		mvp = projection * view * model;
		shader.SetUniformMatrix4fv("mvp", mvp);
		shader.SetUniformMatrix4fv("model", model);

		color = glm::vec4(0.6f/3, 0.4f/3, 0.2f, 1.0f);
		shader.SetUniform4fv("color", color);
		
		normal = glm::transpose(glm::inverse(model));
		shader.SetUniformMatrix4fv("normalMatrix", normal);

		renderer.DrawArrays(va, vb, shader);

		//prepare for switching framebuffers
		glDepthMask(GL_FALSE);
		glEnable(GL_BLEND);
		glBlendFunci(0, GL_ONE, GL_ONE);
		glBlendFunci(1, GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
		glBlendEquation(GL_FUNC_ADD);

		glBindFramebuffer(GL_FRAMEBUFFER, transparentFB);

		glClearBufferfv(GL_COLOR, 0, &zeroFillerVec[0]);
		glClearBufferfv(GL_COLOR, 1, &oneFillerVec[0]);

		experimental.Bind();

		model = glm::mat4(1.0f);

		experimental.SetUniform1f("near", near);
		experimental.SetUniform1f("far", far);

		//glStencilMask(0x00);

		//subject.Draw(experimental, cam.GetCamPosition());

		//glStencilMask(0xFF);

		experimental.SetUniform3fv("viewPosition", cam.GetCamPosition());

		experimental.SetUniform3fv("dirLight.color", dirLightColor);
		experimental.SetUniform3fv("dirLight.direction",  dirLightDirection);
		experimental.SetUniform3fv("dirLight.ambient",	dirLightAmbient);
		experimental.SetUniform3fv("dirLight.diffuse",	dirLightDiffuse); 
		experimental.SetUniform3fv("dirLight.specular",	dirLightSpecular);

		experimental.SetUniform3fv("pointLight.color", pointLightColor);
		experimental.SetUniform3fv("pointLight.position", pointLightPosition);
		experimental.SetUniform3fv("pointLight.ambient", pointLightAmbient);
		experimental.SetUniform3fv("pointLight.diffuse", pointLightDiffuse);
		experimental.SetUniform3fv("pointLight.specular", pointLightSpecular);
		experimental.SetUniform3fv("pointLight.atten", attenuationParams);


		//for multiple lights
		/*for (int i = 0; i < 4; i++)
		{
			experimental.SetUniform3fv(std::string("pointLight[") + std::to_string(i) + std::string("].color"),	pointLightColor);
			experimental.SetUniform3fv(std::string("pointLight[") + std::to_string(i) + std::string("].position"), pointLightPositions[i]);
			experimental.SetUniform3fv(std::string("pointLight[") + std::to_string(i) + std::string("].ambient"),	pointLightAmbient);
			experimental.SetUniform3fv(std::string("pointLight[") + std::to_string(i) + std::string("].diffuse"),	pointLightDiffuse);
			experimental.SetUniform3fv(std::string("pointLight[") + std::to_string(i) + std::string("].specular"), pointLightSpecular);
			experimental.SetUniform3fv(std::string("pointLight[") + std::to_string(i) + std::string("].atten"), attenuationParams);
		}*/

		//experimental.SetUniform3fv("mat.diffuse", matDiffuse);
		//experimental.SetUniform3fv("mat.specular", matSpecular);

		experimental.SetUniform1f("mat.shininess", matShininess);

		//setting up for transparent experimental

		//transparent rendering begins

		for (int i = 0; i < 5; i++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, cubepositions[i]);

			color = glm::vec4(0.6/(i+1), 0.4/(1+i), 0.1*i, 0.4);

			mvp = projection * view * model;
			experimental.SetUniformMatrix4fv("mvp", mvp);
			experimental.SetUniformMatrix4fv("model", model);

			experimental.SetUniform4fv("color", color);
			normal = glm::transpose(glm::inverse(model));
			experimental.SetUniformMatrix4fv("normalMatrix", normal);

			if(i != 2) renderer.DrawArrays(va, vb, experimental);
		}

		glDepthFunc(GL_ALWAYS);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glBindFramebuffer(GL_FRAMEBUFFER, opaqueFB);

		//transparent rendering ends

		// use composite shader
		compositeShader.Bind();

		// draw screen quad
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, accumTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, revealTexture);
		quadVA.Bind();
		glDrawArrays(GL_TRIANGLES, 0, 6);
		
		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE); //<-- enable depth writes so glClear clears the depth buffer
		glDisable(GL_BLEND);
		
		// bind backbuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		
		// use screen shader
		screenShader.Bind();
		
		// draw final screen quad
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, opaqueBuffer);
		quadVA.Bind();
		glDrawArrays(GL_TRIANGLES, 0, 6);

		//multiple lights
		/*for (int i = 0; i < 4; i++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, pointLightPositions[i]);

			pointLightPositions[i].x = radius * glm::sin(glfwGetTime()) * glm::pow(-1, i);
			pointLightPositions[i].z = radius * glm::cos(glfwGetTime());

			model = glm::scale(model, glm::vec3(0.2f));
			mvp = projection * view * model;
			lightShader.SetUniformMatrix4fv("mvp", mvp);
			renderer.DrawArrays(lightVA, vb, lightShader);
		}
		lightShader.SetUniform3fv("lightColor", pointLightColor);*/

		//imgui
		{
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin("color");

			ImGui::SliderFloat("near", &near, 0.0f, 1.0f);
			ImGui::SliderFloat("far", &far, 50.0f, 100.0f);
			ImGui::ColorEdit3("lightColor",  (float*)&pointLightColor);
			//ImGui::InputInt("Kuwahara Radius", &kuwahara_radius);
			
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		//swap buffers and poll events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow * window, int width, int height)
{
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
}

void processInput(GLFWwindow * window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	float lightSpeed = 5.0f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cam.ProcessMovement(cam.FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cam.ProcessMovement(cam.BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cam.ProcessMovement(cam.RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cam.ProcessMovement(cam.LEFT, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		pointLightPosition += cam.GetCamFront() * lightSpeed;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		pointLightPosition -= cam.GetCamFront() * lightSpeed;
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		pointLightPosition -= cam.GetCamRight() * lightSpeed;
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		pointLightPosition += cam.GetCamRight() * lightSpeed;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		pointLightPosition.y += lightSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
		pointLightPosition.y -= lightSpeed;
	}

	if (glfwGetKey(window, GLFW_KEY_GRAVE_ACCENT) == GLFW_PRESS && inputFlag == 0) 
	{
		switch (cameraLock) {
		case 0:	
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			cameraLock = 1;
			mouseCtrl = 1;
			break;

		case 1:
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			cameraLock = 0;
			mouseCtrl = 0;
			cam.SetMouseFirst();
			break;
		}
		
		inputFlag = 1;
	}
	if (glfwGetKey(window, GLFW_KEY_GRAVE_ACCENT) == GLFW_RELEASE)
		inputFlag = 0;
}

void mouse_callback(GLFWwindow * window, double xPos, double yPos)
{
	if(mouseCtrl == 1)
		cam.ProcessMouseInput(xPos, yPos);
}

void scroll_callback(GLFWwindow * window, double xOffset, double yOffset)
{
	cam.ProcessScroll(xOffset, yOffset);
}
