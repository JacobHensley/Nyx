#pragma once
#include "Nyx/Core/Core.h"
#include "Nyx/scene/Scene.h"
#include "Nyx/Graphics/Mesh.h"
#include "Nyx/Graphics/Material.h"
#include "Nyx/graphics/Camera.h"

namespace Nyx {

	class FullscreenQaud
	{
	public:
		FullscreenQaud()
		{
			m_VertexArray = CreateRef<VertexArray>();

			struct QuadVertex
			{
				glm::vec2 Position;
				glm::vec2 TexCoord;
			};

			std::array<QuadVertex, 4> vertices;

			vertices[0].Position = { -1.0f, -1.0f };
			vertices[0].TexCoord = { 0.0f, 0.0f };

			vertices[1].Position = { 1.0f, -1.0f };
			vertices[1].TexCoord = { 1.0f, 0.0f };

			vertices[2].Position = { 1.0f,  1.0f };
			vertices[2].TexCoord = { 1.0f, 1.0f };

			vertices[3].Position = { -1.0f,  1.0f };
			vertices[3].TexCoord = { 0.0f, 1.0f };

			Ref<VertexBuffer> vertexBuffer = CreateRef<VertexBuffer>(vertices.data(), vertices.size() * sizeof(QuadVertex));
			m_VertexArray->PushVertexBuffer(vertexBuffer);

			uint32_t fullscreenQuadIndices[] = { 0, 1, 2, 2, 3, 0 };
			m_IndexBuffer = CreateRef<IndexBuffer>(fullscreenQuadIndices, 6);
		}

		void Bind()
		{
			m_VertexArray->Bind();
			m_IndexBuffer->Bind();
		}

		void Unbind()
		{
			m_VertexArray->Unbind();
			m_IndexBuffer->Unbind();
		}

	private:
		Ref<VertexArray> m_VertexArray;
		Ref<IndexBuffer> m_IndexBuffer;
	};

	class UniformBuffer;

	class Renderer
	{
	public:
		static void Init();
		static void Begin(Ref<Camera> camera, Ref<EnvironmentMap> environmentMap);
		static void End();

		static void SubmitMesh(Ref<Mesh> mesh, glm::mat4 transform, Ref<Material> materialOverride);
		static void SubmitFullscreenQuad(Ref<Material> material);
		
	private:
		void InitRendererUniformFunctions();
		void UploadUniformBuffer(const Ref<UniformBuffer>& uniformBuffer, byte* buffer);
	};
}