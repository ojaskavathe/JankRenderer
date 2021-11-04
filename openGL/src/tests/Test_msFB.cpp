#include "Test_msFB.h"
#include "VertexBufferLayout.h"
#include "imgui/imgui.h"

test::Test_msFB::Test_msFB()
	:shader("res/shaders/basicv.vert", "res/shaders/basicf.frag"),
	 FBshader("res/shaders/screenShaderv.vert", "res/shaders/screenShaderf.frag")
{
	//init arrays and buffers
	VertexBuffer vb(vertices, (unsigned int)sizeof(vertices));
	VertexBufferLayout layout;

	//position
	layout.Push<float>(3);

	//normals
	layout.Push<float>(3);

	//textCoords
	layout.Push<float>(2);

	va.AddBuffer(vb, layout);
	va.Unbind();

	VertexBuffer quadVB(quadVerts, (unsigned int)sizeof(quadVerts));
	VertexBufferLayout quadLayout;

	quadLayout.Push<float>(2);
	quadLayout.Push<float>(2);

	quadVA.AddBuffer(quadVB, quadLayout);
	quadVA.Unbind();

	stbi_set_flip_vertically_on_load(true);

	FB.GenTextureBufferMS(fbTexture, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, GL_COLOR_ATTACHMENT0, 4);
	FB.CheckStatus();

	screenFB.GenTextureBuffer(screenTex, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, GL_COLOR_ATTACHMENT0);

	//wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	//glEnable(GL_PROGRAM_POINT_SIZE);

	//backface culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}

test::Test_msFB::~Test_msFB() {}

void test::Test_msFB::OnUpdate(float deltaTime, GLFWwindow* window)
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

void test::Test_msFB::OnRender()
{
	FB.Bind();

	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	projection = glm::perspective(glm::radians(cam.GetFov()), 800.0f / 600.0f, near, far);
	view = cam.GetViewMatrix();
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -5.0f));

	mvp = projection * view * model;

	shader.Bind();
	shader.SetUniformMatrix4fv("mvp", mvp);
	shader.SetUniform3fv("color", glm::vec3(1.0f, 0.0f, 0.0f));

	va.Bind();
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, FB.getID());
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, screenFB.getID());
	glBlitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	
	glDisable(GL_DEPTH_TEST);
	quadVA.Bind();
	FBshader.Bind();
	FBshader.SetUniform1f("gamma", 2.2f);
	FBshader.SetUniform1f("exposure", 1.0f);
	glBindTexture(GL_TEXTURE_2D, screenTex);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void test::Test_msFB::OnImGuiRender()
{

}

void test::Test_msFB::CursorInput(double xPos, double yPos)
{
	if (mouseCtrl == 1)
		cam.ProcessMouseInput(xPos, yPos);
}

void test::Test_msFB::ScrollInput(double xOffset, double yOffset)
{
	cam.ProcessScroll(xOffset, yOffset);
}