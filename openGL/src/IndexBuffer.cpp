#include "IndexBuffer.h"
#include <glad/gl.h>

IndexBuffer::IndexBuffer()
{
}

IndexBuffer::IndexBuffer(const IndexBuffer& ibo)
{
	m_RendererID = ibo.getID();
	m_Count = ibo.getCount();
}

IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int count)
	: m_Count(count)
{
	glGenBuffers(1, &m_RendererID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW);
}

IndexBuffer::~IndexBuffer()
{
	//glDeleteBuffers(1, &m_RendererID);
}

void IndexBuffer::BindData(const unsigned int * data, unsigned int count)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW);
}

void IndexBuffer::Bind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
}

void IndexBuffer::Unbind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void IndexBuffer::Delete() const
{
	glDeleteBuffers(1, &m_RendererID);
}