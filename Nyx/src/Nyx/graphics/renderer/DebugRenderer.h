#pragma once

#include "Nyx/graphics/API/IndexBuffer.h"
#include "Nyx/graphics/API/VertexArray.h"
#include "Nyx/graphics/API/VertexBuffer.h"

#include "Nyx/graphics/renderer/Shader.h"

#include "Nyx/math/AABB.h"

#include <glm/glm.hpp>

namespace Nyx {

	class DebugRenderer
	{
		struct LineVertex
		{
			glm::vec3 Position;
			glm::vec4 Color;
		};

	public:
		DebugRenderer();
		~DebugRenderer();

	public:
		static void Init();

		static void Begin(const Camera& camera) { s_Instance->BeginInternal(camera); }
		static void End() { s_Instance->EndInternal(); }
		static void Flush() { s_Instance->FlushInternal(); }

		static void DrawLine(const glm::vec3& v0, const glm::vec3& v1, const glm::vec4& color = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f)) { s_Instance->DrawLineInternal(v0, v1, color); }
		static void DrawAABB(const AABB& aabb, const glm::mat4& transform, const glm::vec4& color = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f)) { s_Instance->DrawAABBInternal(aabb, transform, color); }

	private:
		void BeginInternal(const Camera& camera);
		void EndInternal();
		void FlushInternal();

		void DrawLineInternal(const glm::vec3& v0, const glm::vec3& v1, const glm::vec4& color);
		void DrawAABBInternal(const AABB& aabb, const glm::mat4& transform, const glm::vec4& color);

	private:
		LineVertex* m_LineVertexBuffer = nullptr;
		LineVertex* m_LineVertexBufferWritePtr = nullptr;

		std::unique_ptr<Shader> m_LineShader;

		std::shared_ptr<VertexArray> m_VertexArray;
		std::shared_ptr<VertexBuffer> m_VertexBuffer;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;

		uint m_LineIndexCount = 0;

	private:
		static DebugRenderer* s_Instance;
	};

}
