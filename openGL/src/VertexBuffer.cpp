#include "VertexBuffer.h"
#include <glad/gl.h>

VertexBuffer::VertexBuffer()
{
}

VertexBuffer::VertexBuffer(const void * data, unsigned int size)
{
	glGenBuffers(1, &m_RendererID);
	glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

VertexBuffer::VertexBuffer(const VertexBuffer& vbo)
{
	m_RendererID = vbo.GetID();
}

VertexBuffer::~VertexBuffer()
{
	//buffer gets deleted at the end of its scope which doesn't work with radeon drivers but somehow works with nvidia drivers???
	//glDeleteBuffers(1, &m_RendererID);
}

void VertexBuffer::BindData(const void * data, unsigned int size)
{
	glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

void VertexBuffer::Bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
}

void VertexBuffer::Unbind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::Delete() const
{
	glDeleteBuffers(1, &m_RendererID);
}