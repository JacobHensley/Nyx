#include "NXpch.h"
#include "IndexBuffer.h"
#include "glad/glad.h"

namespace Nyx {

	IndexBuffer::IndexBuffer()
	{
		glGenBuffers(1, &m_BufferID);
	}

	IndexBuffer::IndexBuffer(const uint* data, uint count)
		: m_Count(count)
	{
		glGenBuffers(1, &m_BufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint), data, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	IndexBuffer::~IndexBuffer()
	{
		glDeleteBuffers(1, &m_BufferID);
	}

	void IndexBuffer::SetData(const uint* data, uint count)
	{
		m_Count = count;
		Bind();
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint), data, GL_STATIC_DRAW);
		Unbind();
	}

	void IndexBuffer::Draw() const
	{
		Draw(m_Count);
	}

	void IndexBuffer::Draw(uint count) const
	{
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, NULL);
	}

	void IndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID);
	}

	void IndexBuffer::Unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

}