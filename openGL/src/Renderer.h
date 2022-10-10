#pragma once
#include <memory>
#include <glad/glad.h>

#define ASSERT(x) if (!(x)) __debugbreak();
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

void SetDebugCallback();

class VertexArray;
class Shader;
class IndexBuffer;

class Renderer
{
public:
	const void Clear() const;
	const void DrawArrays(VertexArray& va, Shader& shader) const;
	const void DrawElements(VertexArray& va, IndexBuffer& ib, Shader& shader) const;

};