#include <iostream>
#include "Renderer.h"
#include <stb_image/stb_image.h>

void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	static std::string init = ""; //is this really the best solution
	if (message != init && severity != GL_DEBUG_SEVERITY_HIGH && severity != GL_DEBUG_SEVERITY_NOTIFICATION) {
		std::cout << "[openGL]: " << message << std::endl;
		init = message;
	}

	if (message != init && severity == GL_DEBUG_SEVERITY_HIGH) {
		std::cout << "[openGL]: " << message << std::endl;
		init = message;
	}
}

void SetDebugCallback()
{
	glDebugMessageCallback(&MessageCallback, 0);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
}


const void Renderer::Clear() const
{
	glClearColor(0.3f, 0.2f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

const void Renderer::DrawArrays(VertexArray& va, Shader& shader) const
{
	shader.Bind();
	va.Bind();
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

const void Renderer::DrawElements(VertexArray & va, IndexBuffer & ib, Shader & shader) const
{
	shader.Bind();
	va.Bind();
	ib.Bind();
	glDrawElements(GL_TRIANGLES, ib.getCount(), GL_UNSIGNED_INT, 0);
}