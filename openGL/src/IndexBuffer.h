#pragma once

class IndexBuffer 
{
private:
	unsigned int m_RendererID = 0;
	unsigned int m_Count = 0;
public:
	IndexBuffer();
	IndexBuffer(const IndexBuffer& ibo);
	IndexBuffer(const unsigned int* data, unsigned int count);
	~IndexBuffer();

	void BindData(const unsigned int* data, unsigned int count);
	void Bind() const;
	void Unbind() const;

	void Delete() const;

	inline unsigned int getID() const { return m_RendererID; }
	inline unsigned int getCount() const { return m_Count; }
};