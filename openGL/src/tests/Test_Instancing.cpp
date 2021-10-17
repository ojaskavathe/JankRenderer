#include "Test_Instancing.h"
#include "VertexBufferLayout.h"
#include "imgui/imgui.h"

test::Test_Instancing::Test_Instancing()
	:shader("res/shaders/basicv.vert", "res/shaders/basicf.frag"),
	 modelMatrices(new glm::mat4[amount]),
	 color(new glm::vec3[amount])
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

	srand(glfwGetTime());
	float radius = 10.0f;
	float offset = 2.5f;
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
		float scale = (rand() % 20) / 100.0f + 0.05;
		model = glm::scale(model, glm::vec3(scale));

		// 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
		float rotAngle = (rand() % 360);
		model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

		// 4. now add to list of matrices
		modelMatrices[i] = model;
		color[i] = glm::vec3(rand()/2550, rand()/2550, rand()/2550);
	}

	//glVertexAttribDivisor(2, 1);

	va.Unbind();

	//wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	glEnable(GL_PROGRAM_POINT_SIZE);

	//backface culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}

test::Test_Instancing::~Test_Instancing(){}

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
	projection = glm::perspective(glm::radians(cam.GetFov()), 800.0f / 600.0f, near, far);
	view = cam.GetViewMatrix();
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
	model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));

	va.Bind();
	shader.Bind();
	for (unsigned int i = 0; i < amount; i++)
	{
		model = modelMatrices[i];
		mvp = projection * view * model;
		shader.SetUniformMatrix4fv("mvp", mvp);
		shader.SetUniform3fv("color", color[i]);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}


	//glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 100);
}

void test::Test_Instancing::OnImGuiRender()
{
	ImGui::Begin("color");

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