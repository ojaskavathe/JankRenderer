#pragma once
#include "Test.h"

namespace test {
	class Test_ClearColor : public Test
	{
	public:
		Test_ClearColor();
		~Test_ClearColor();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;

	private:
		float m_ClearColor[4];
	};
}