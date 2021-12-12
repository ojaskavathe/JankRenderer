#include "Test_Instancing.h"
#include "VertexBufferLayout.h"
#include "imgui/imgui.h"
#include <random>

test::Test_Instancing::Test_Instancing()
	:shader("res/shaders/instancingv.vert", "res/shaders/basicf.frag")
{
	//init arrays and buffers
	VertexBuffer vb(vertices, unsigned int(sizeof(vertices)));
	VertexBufferLayout layout;

	//position
	layout.Push<float>(3);

	//normals
	layout.Push<float>(3);

	//textCoords
	layout.Push<float>(2);

	va.AddBuffer(vb, layout);

	//std::vector<glm::mat4> modelMatrices;
	glm::mat4* modelMatrices;
	modelMatrices = new glm::mat4[amount];
	//std::vector<glm::vec3> modelColor;
	glm::vec3* modelColor;
	modelColor = new glm::vec3[amount];

	//set rng
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> distr(0, 1000);

	srand((unsigned int)glfwGetTime());
	float radius = 50.0f;
	float offset = 25.0f;
	for (unsigned int i = 0; i < amount; i++)
	{
		glm::mat4 model = glm::mat4(1.0f);
		// 1. translation: displace along circle with 'radius' in range [-offset, offset]
		float angle = (float)i / (float)amount * 360.0f;
		float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float x = sin(angle) * radius + displacement;
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float y = displacement * 0.4f; // keep height of field smaller compared to width of x and z
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float z = cos(angle) * radius + displacement;
		model = glm::translate(model, glm::vec3(x, y, z));

		// 2. scale: scale between 0.05 and 0.25f
		float scale = (float)((rand() % 20) / 100.0f + 0.05);
		model = glm::scale(model, glm::vec3(scale));

		// 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
		float rotAngle = (float)(rand() % 360);
		model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

		// 4. now add to list of matrices
		//modelMatrices.push_back(model);
		modelMatrices[i] = model;
		//modelColor.push_back(glm::vec3(distr(gen) / 500, distr(gen) / 500, distr(gen) / 500));
		modelColor[i] = (glm::vec3(distr(gen) / 500, distr(gen) / 500, distr(gen) / 500));
	}

	cubeInstanceVB.BindData(modelMatrices, (amount) * sizeof(glm::mat4));

	layout.Push<float>(4);
	layout.Push<float>(4);
	layout.Push<float>(4);
	layout.Push<float>(4);
	va.AddBuffer(cubeInstanceVB, layout);

	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
	glVertexAttribDivisor(6, 1);

	VertexBuffer cubeColorVB(modelColor, (amount) * sizeof(glm::vec3));
	layout.Push<float>(3);
	va.AddBuffer(cubeColorVB, layout);
	
	glVertexAttribDivisor(7, 1);

	va.Unbind();

	//wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//backface culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}

test::Test_Instancing::~Test_Instancing(){
	glDeleteVertexArrays(1, va.GetID());
}

void test::Test_Instancing::OnUpdate(float deltaTime, GLFWwindow* window)
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

void test::Test_Instancing::OnRender()
{
	view = cam.GetViewMatrix();

	vp = projection * view;

	va.Bind();
	shader.Bind();

	shader.SetUniformMatrix4fv("vp", vp);

	glDrawArraysInstanced(GL_TRIANGLES, 0, 36, amount);
}

void test::Test_Instancing::OnImGuiRender()
{
	ImGui::Begin("Instancing");

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();
}

void test::Test_Instancing::CursorInput(double xPos, double yPos)
{
	if(mouseCtrl == 1)
		cam.ProcessMouseInput(xPos, yPos);
}

void test::Test_Instancing::ScrollInput(double xOffset, double yOffset)
{
	cam.ProcessScroll(xOffset, yOffset);
}