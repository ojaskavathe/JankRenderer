#include <iostream>

#include <Renderer.h>
#include "Settings.h"
#include "Camera.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"

#include "tests/Test_Compute.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>

// #include <windows.h> // for nvidia optimus

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xPos, double yPos);
void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);

//for callbacks, this is the only way to get the test i found
test::Test* mTest;

float deltaTime = 0.0f;
float LastFrame = 0.0f;

// extern "C" {
// 	_declspec(dllexport) DWORD NvOptimusEnablement = 1;
// 	_declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
// }

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_SAMPLES, 4);// <- Add samples for MSAA

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

	// Load OpenGL functions, gladLoadGL returns the loaded version, 0 on error.
    int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0)
    {
        std::cout << "Failed to initialize OpenGL context" << std::endl;
        return -1;
    }

	//enable gl debugging
	SetDebugCallback();

	//set current test
	test::Test_Compute test1;

	mTest = &test1;
	Renderer renderer;

	const char* glsl_version = "#version 460";
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	glEnable(GL_MULTISAMPLE);

	//https://developer.nvidia.com/content/depth-precision-visualized
	// glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE); //set up for depth precision control

	std::cout << glGetString(GL_RENDERER) << "\n";

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