#include "VertexArray.h"

VertexArray::VertexArray()
	:currentPos(0)
{
	glGenVertexArrays(1, &m_RendererID);
	glBindVertexArray(m_RendererID);
}

VertexArray::~VertexArray()
{
	//vertex arrays have to last longer than their scope usually so delete them manually if you have to
	//glDeleteVertexArrays(1, &m_RendererID);
}

void VertexArray::AddBuffer(const VertexBuffer& vb, VertexBufferLayout& layout)
{
	Bind();
	vb.Bind();
	const auto& elements = layout.getElements();
	unsigned int offset = 0;

	for (unsigned int i = currentPos; i < elements.size(); i++)
	{
		const auto& element = elements[i];
		glEnableVertexAttribArray(i);
		glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.getStride(), (const void*)((size_t)offset));
		offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
		currentPos++;
	}
	layout.resetStride(); //-> if we add attributes from another buffer
}

void VertexArray::Bind() const
{
	glBindVertexArray(m_RendererID);
}

void VertexArray::Unbind() const
{
	glBindVertexArray(0);
}

void VertexArray::Delete() const
{
	glDeleteVertexArrays(1, &m_RendererID);
}
