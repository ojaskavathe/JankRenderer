#include "Test_ClearColor.h"
#include "Renderer.h"
#include "imgui/imgui.h"

test::Test_ClearColor::Test_ClearColor()
	: m_ClearColor{0.2f, 0.1f, 0.6f, 1.0f}
{

}

test::Test_ClearColor::~Test_ClearColor()
{
}

void test::Test_ClearColor::OnUpdate(float deltaTime)
{
}

void test::Test_ClearColor::OnRender()
{
	glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]);
	glClear(GL_COLOR_BUFFER_BIT);
}

void test::Test_ClearColor::OnImGuiRender()
{
	ImGui::ColorEdit4("Clear Color", m_ClearColor);
}
