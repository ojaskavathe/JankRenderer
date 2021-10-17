#include "Test_Instancing.h"
#include "VertexBufferLayout.h"
#include "imgui/imgui.h"

test::Test_Instancing::Test_Instancing()
	:shader("res/shaders/instancingv.vert", "res/shaders/basicf.frag")
{
	//init arrays and buffers
	VertexBuffer vb(vertices, unsigned int(sizeof(vertices)));
	VertexBufferLayout layout;

	//position
	//layout.Push<float>(3);
	layout.Push<float>(2);

	//normals
	//layout.Push<float>(3);

	//textCoords
	//layout.Push<float>(2);

	//color
	layout.Push<float>(3);
	vb.Unbind();

	va.AddBuffer(vb, layout);

	//positions
	glm::vec2 translations[100];
	int index = 0;
	float offset = 0.1f;
	for (int y = -10; y < 10; y += 2) {
		for (int x = -10; x < 10; x += 2) {
			glm::vec2 translation;
			translation.x = (float)x / 10.0f + offset;
			translation.y = (float)y / 10.0f + offset;
			translations[index++] = translation;
		}
	}

	VertexBuffer instanceVB(&translations[0], unsigned int(sizeof(translations)));

	//glEnableVertexAttribArray(2);
	//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (const void*)0);

	layout.Push<float>(2);
	va.AddBuffer(instanceVB, layout);

	glVertexAttribDivisor(2, 1);

	va.Unbind();

	//wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	glEnable(GL_PROGRAM_POINT_SIZE);

	//backface culling
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
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
	//projection = glm::perspective(glm::radians(cam.GetFov()), 800.0f / 600.0f, near, far);
	//view = cam.GetViewMatrix();
	//model = glm::mat4(1.0f);
	//model = glm::translate(model, glm::vec3(0.0f, 0.0f, -5.0f));
	//
	//mvp = projection * view * model;
	//
	shader.Bind();
	//shader.SetUniformMatrix4fv("mvp", mvp);
	//shader.SetUniform3fv("color", glm::vec3(1.0f, 0.0f, 0.0f));

	va.Bind();
	//glDrawArrays(GL_TRIANGLES, 0, 36);
	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 100);
}

void test::Test_Instancing::OnImGuiRender()
{
	
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