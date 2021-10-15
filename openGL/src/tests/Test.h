#pragma once
#include <GLFW/glfw3.h>

namespace test {
	class Test
	{
	public:
		Test() {}
		virtual ~Test() {}
		
		virtual void OnUpdate(float deltaTime, GLFWwindow* window) {}
		virtual void OnRender() {}
		virtual void OnImGuiRender() {}

		virtual void CursorInput(double xPos, double yPos) {}
		virtual void ScrollInput(double xOffset, double yOffset) {}
	};
}