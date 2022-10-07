#pragma once
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

class VertexArray
{
private:
	unsigned int m_RendererID = 0;
	unsigned int currentPos = 0;
public:
	VertexArray();
	~VertexArray();

	void AddBuffer(const VertexBuffer& vb, VertexBufferLayout& layout);

	void Bind() const;
	void Unbind() const;
	void Delete() const;
	inline const unsigned int* GetID() const { return &m_RendererID; }
};