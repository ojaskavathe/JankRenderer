#pragma once
#include <glad\glad.h>
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Shader.h"

#define ASSERT(x) if (!(x)) __debugbreak();
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

void SetDebugCallback();

class Renderer
{
public:
	void Clear() const;
	void DrawArrays(VertexArray& va, Shader& shader) const;
	void DrawElements(VertexArray& va, IndexBuffer& ib, Shader& shader) const;
};