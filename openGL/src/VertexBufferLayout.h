#pragma once

#include <vector>
#include <glad/gl.h>

struct VertexBufferElement 
{
	unsigned int type;
	unsigned int count;
	unsigned char normalized;

	static unsigned int GetSizeOfType(unsigned int type)
	{
		switch (type)
		{
		case GL_FLOAT:			return 4;
		case GL_UNSIGNED_INT:	return 4;
		case GL_UNSIGNED_BYTE:	return 1;
		}
		__debugbreak();
		return 0;
	}
};

class VertexBufferLayout 
{
private:
	std::vector<VertexBufferElement> m_Elements;
	unsigned int m_Stride;
public:
	VertexBufferLayout()
		:m_Stride(0) {}

	//https://stackoverflow.com/questions/14637356/static-assert-fails-compilation-even-though-template-function-is-called-nowhere
	template<typename T>
	struct foobar : std::false_type
	{ };

	template<typename T>
	void Push(unsigned int count) 
	{
		static_assert(foobar<T>::value, "Invalid Type of Vertex Buffer Element");
	}
	
	template<>
	void Push<float>(unsigned int count)
	{
		m_Elements.push_back({ GL_FLOAT, count, GL_FALSE });
		m_Stride += count * VertexBufferElement::GetSizeOfType(GL_FLOAT);
	}

	template<>
	void Push<unsigned int>(unsigned int count)
	{
		m_Elements.push_back({ GL_UNSIGNED_INT, count, GL_FALSE });
		m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT);
	}

	template<>
	void Push<unsigned char>(unsigned int count) 
	{
		m_Elements.push_back({ GL_UNSIGNED_BYTE, count, GL_TRUE });
		m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE);
	}

	inline unsigned int getStride() const { return m_Stride; }
	inline const std::vector<VertexBufferElement> getElements() const& { return m_Elements; }
	inline void strideOverride(int strideOffset, unsigned int type) { m_Stride += strideOffset * VertexBufferElement::GetSizeOfType(type);  }
	inline void resetStride() { m_Stride = 0; }
};
