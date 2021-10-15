#pragma once
#include "Test.h"

namespace test {
	class Test_ClearColor : public Test
	{
	public:
		Test_ClearColor();
		~Test_ClearColor();

		void OnUpdate(float deltaTime, GLFWwindow* window) override;
		void OnRender() override;
		void OnImGuiRender() override;
		
		void CursorInput(double xPos, double yPos) override;
		void ScrollInput(double xOffset, double yOffset) override;

	private:
		float m_ClearColor[4];
	};
}