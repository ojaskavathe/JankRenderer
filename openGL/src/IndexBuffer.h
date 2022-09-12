#pragma once

class IndexBuffer 
{
private:
	unsigned int m_RendererID;
	unsigned int m_Count;
public:
	IndexBuffer();
	IndexBuffer(const unsigned int* data, unsigned int count);
	~IndexBuffer();

	void BindData(const unsigned int* data, unsigned int count);
	void Bind() const;
	void Unbind() const;

	void Delete() const;

	inline unsigned int getCount() const { return m_Count; }
};