#include <glad\glad.h>
#include <GLFW\glfw3.h>

#include <iostream>

#include "Camera.h"

#include "Renderer.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"

#include "tests/Test_ClearColor.h"
#include "tests/Test_Transparency.h"
#include "tests/Test_Points.h"
#include "tests/Test_GeometryShader.h"
#include "tests/Test_Instancing.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xPos, double yPos);
void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);

//for callbacks, this is the only way to get the test i found
test::Test* mTest;

float deltaTime = 0.0f;
float LastFrame = 0.0f;

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
	test::Test_Instancing test1;

	mTest = &test1;
	//test::Test_ClearColor test1;
	Renderer renderer;

	const char* glsl_version = "#version 460";
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	while (!glfwWindowShouldClose(window))
	{	
		//setDeltaTime
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - LastFrame;
		LastFrame = currentFrame;

		renderer.Clear();

		test1.OnUpdate(deltaTime, window);
		test1.OnRender();

		//Init imgui
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		test1.OnImGuiRender();

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

void mouse_callback(GLFWwindow * window, double xPos, double yPos)
{
	(*mTest).CursorInput(xPos, yPos);
}

void scroll_callback(GLFWwindow * window, double xOffset, double yOffset)
{
	(*mTest).ScrollInput(xOffset, yOffset);
}