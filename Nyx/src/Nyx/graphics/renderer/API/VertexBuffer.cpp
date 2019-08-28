#include "NXpch.h"
#include "VertexBuffer.h"
#include "glad/glad.h"

namespace Nyx {

	VertexBuffer::VertexBuffer()
	{
		glGenBuffers(1, &m_BufferID);
	}

	VertexBuffer::VertexBuffer(uint size)
	{
		glGenBuffers(1, &m_BufferID);
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
		glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	VertexBuffer::VertexBuffer(void* data, int size)
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

	void VertexBuffer::SetData(float* data, int size)
	{
		Bind();
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
		Unbind();
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