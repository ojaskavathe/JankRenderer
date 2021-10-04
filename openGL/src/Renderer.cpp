#include "Renderer.h"
#include <iostream>

std::string init = "";
void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	if (message != init && severity != GL_DEBUG_SEVERITY_NOTIFICATION) {
		std::cout << "[openGL]: " << message << std::endl;
		init = message;
	}
}

void SetDebugCallback()
{
	glDebugMessageCallback(&MessageCallback, 0);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
}

void Renderer::Clear() const
{
	glClearColor(0.3f, 0.2f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void Renderer::DrawArrays(VertexArray& va, VertexBuffer& vb, Shader& shader) const
{
	shader.Bind();
	va.Bind();
	vb.Bind();
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void Renderer::DrawElements(VertexArray & va, IndexBuffer & ib, Shader & shader) const
{
	shader.Bind();
	va.Bind();
	ib.Bind();
	glDrawElements(GL_TRIANGLES, ib.getCount(), GL_UNSIGNED_INT, 0);
}
