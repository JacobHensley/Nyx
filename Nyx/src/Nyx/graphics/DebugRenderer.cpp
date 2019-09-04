#include "NXpch.h"
#include "DebugRenderer.h"

#include <glad/glad.h>

namespace Nyx {

	DebugRenderer* DebugRenderer::s_Instance = nullptr;

	void DebugRenderer::Init()
	{
		NX_ASSERT(s_Instance == nullptr, "Already initialized!");
		s_Instance = new DebugRenderer;
	}

	DebugRenderer::DebugRenderer()
	{
		const uint MAX_LINE_COUNT = 1024;
		const uint MAX_INDEX_COUNT = MAX_LINE_COUNT * 2;

		m_LineVertexBuffer = new LineVertex[1024];
		m_VertexArray = std::make_unique<VertexArray>();

		uint* indices = new uint[MAX_INDEX_COUNT];
		for (uint i = 0; i < MAX_INDEX_COUNT; i++)
			indices[i] = i;
		
		m_VertexArray = std::make_unique<VertexArray>();
		m_VertexBuffer = std::make_unique<VertexBuffer>((uint)(MAX_LINE_COUNT * sizeof(LineVertex) * 2));
		m_VertexBuffer->SetLayout({
			{ ShaderDataType::Vec3, "a_Position"},
			{ ShaderDataType::Vec4, "a_Color"}
		});
		m_VertexArray->PushVertexBuffer(m_VertexBuffer.get());
		m_IndexBuffer = std::make_unique<IndexBuffer>(indices, MAX_INDEX_COUNT);
		m_IndexBuffer->SetData(indices, MAX_INDEX_COUNT);

		m_LineShader = std::make_unique<Shader>("res/shaders/DebugLine.shader");
	} 

	DebugRenderer::~DebugRenderer()
	{
		delete[] m_LineVertexBuffer;
	}

	void DebugRenderer::BeginInternal(const Camera& camera)
	{
		m_LineIndexCount = 0;
		m_LineVertexBufferWritePtr = m_LineVertexBuffer;

		m_LineShader->Bind();
		m_LineShader->SetUniformMat4("u_VP", camera.GetProjectionMatrix() * camera.GetViewMatrix());
	}

	void DebugRenderer::EndInternal()
	{
		std::ptrdiff_t size = (char*)m_LineVertexBufferWritePtr - (char*)m_LineVertexBuffer;
		if (size > 0)
		{
			m_VertexBuffer->SetData(m_LineVertexBuffer, (uint)size);
		}
	}

	void DebugRenderer::FlushInternal()
	{
		m_LineShader->Bind();
		m_VertexArray->Bind();
		m_IndexBuffer->Bind();
		
		//glDisable(GL_DEPTH_TEST);
		glLineWidth(2.0f);
		glDrawElements(GL_LINES, m_LineIndexCount, GL_UNSIGNED_INT, nullptr);
		//glEnable(GL_DEPTH_TEST);
	}

	void DebugRenderer::DrawLineInternal(const glm::vec3& v0, const glm::vec3& v1, const glm::vec4& color)
	{
		NX_ASSERT(m_LineVertexBufferWritePtr - m_LineVertexBuffer < 1024, "Too many vertices!");

		m_LineVertexBufferWritePtr->Position = v0;
		m_LineVertexBufferWritePtr->Color = color;
		m_LineVertexBufferWritePtr++;

		m_LineVertexBufferWritePtr->Position = v1;
		m_LineVertexBufferWritePtr->Color = color;
		m_LineVertexBufferWritePtr++;

		m_LineIndexCount += 2;
	}

	void DebugRenderer::DrawAABBInternal(const AABB& aabb, const glm::mat4& transform, const glm::vec4& color)
	{
		glm::vec3 min = aabb.Min;
		glm::vec3 max = aabb.Max;

		min = glm::vec3(transform * glm::vec4(min, 1.0f));
		max = glm::vec3(transform * glm::vec4(max, 1.0f));
		glm::vec3 size = max - min;

		// Front quad
		DrawLine(min, { min.x + size.x, min.y, min.z }, color);
		DrawLine({ min.x + size.x, min.y, min.z }, { min.x + size.x, min.y + size.y, min.z }, color);
		DrawLine({ min.x + size.x, min.y + size.y, min.z }, { min.x, min.y + size.y, min.z }, color);
		DrawLine({ min.x, min.y + size.y, min.z }, min, color);

		// Join
		DrawLine(min, { min.x, min.y, max.z }, color);
		DrawLine({ max.x, min.y, min.z }, { max.x, min.y, max.z }, color);
		DrawLine({ max.x, max.y, min.z }, { max.x, max.y, max.z }, color);
		DrawLine({ min.x, max.y, min.z }, { min.x, max.y, max.z }, color);

		// Back quad
		DrawLine({ min.x, min.y, max.z }, { min.x + size.x, min.y, max.z }, color);
		DrawLine({ min.x + size.x, min.y, max.z }, { min.x + size.x, min.y + size.y, max.z }, color);
		DrawLine({ min.x + size.x, min.y + size.y, max.z }, { min.x, min.y + size.y, max.z }, color);
		DrawLine({ min.x, min.y + size.y, max.z }, { min.x, min.y, max.z }, color);
	}




}