#pragma once

class VertexBuffer 
{
private:
	unsigned int m_RendererID = 0;

public:
	VertexBuffer();
	VertexBuffer(const void* data, unsigned int size);
	VertexBuffer(const VertexBuffer& vbo);
	~VertexBuffer();

	void BindData(const void* data, unsigned int size);
	void Bind() const ;
	void Unbind() const;
	void Delete() const;
	unsigned int GetID() const { return m_RendererID; }
};