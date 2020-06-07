#include "NXpch.h"
#include "IndexBuffer.h"
#include "glad/glad.h"

namespace Nyx {

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

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint), data, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void IndexBuffer::Draw(bool depthTesting) const
	{
		if (!depthTesting)
			glDisable(GL_DEPTH_TEST);

		glDrawElements(GL_TRIANGLES, m_Count, GL_UNSIGNED_INT, NULL);

		glEnable(GL_DEPTH_TEST);
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