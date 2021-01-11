#include "NXpch.h"
#include "VertexBuffer.h"
#include <glad/glad.h>

namespace Nyx {

	VertexBuffer::VertexBuffer(uint size)
		: m_Size(size)
	{
		glGenBuffers(1, &m_BufferID);
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
		glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	VertexBuffer::VertexBuffer(void* data, int size)
		: m_Size(size)
	{
		glGenBuffers(1, &m_BufferID);
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	VertexBuffer::~VertexBuffer()
	{
		glDeleteBuffers(1, &m_BufferID);
	}

	void VertexBuffer::SetData(const void* data, uint size)
	{
		glNamedBufferSubData(m_BufferID, 0, size, data);
	}

	void VertexBuffer::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
	}

	void VertexBuffer::Unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void VertexBuffer::Unmap()
	{
		glUnmapBuffer(GL_ARRAY_BUFFER);
	}

	void* VertexBuffer::MapInternal()
	{
		void* result = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
		return result;
	}

}