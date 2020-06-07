#include "NXpch.h"
#include "VertexArray.h"
#include "glad/glad.h"

namespace Nyx {

	VertexArray::VertexArray()
	{
		glGenVertexArrays(1, &m_VertexArray);
	}

	VertexArray::~VertexArray()
	{
		glDeleteVertexArrays(1, &m_VertexArray);
	}

	void VertexArray::PushVertexBuffer(Ref<VertexBuffer> vertexBuffer)
	{
		glBindVertexArray(m_VertexArray);
		vertexBuffer->Bind();

		const BufferLayout& layout = vertexBuffer->GetLayout();

		const auto& elements = layout.GetElements();
		uint stride = layout.GetStride();

		for (uint i = 0; i < elements.size(); i++)
		{
			glEnableVertexAttribArray(i);
			glVertexAttribPointer(i, elements[i].GetComponentCount(), ShaderDataTypeToOpenGLBaseType(elements[i].type), GL_FALSE, stride, (const void*)(uint64_t)elements[i].offset);
		}

		vertexBuffer->Unbind();
		glBindVertexArray(0);
	}

	void VertexArray::Bind()
	{
		glBindVertexArray(m_VertexArray);
	}

	void VertexArray::Unbind()
	{
		glBindVertexArray(0);
	}

}