#pragma once
#include "Nyx/Core/Core.h"
#include "Nyx/scene/Scene.h"
#include "Nyx/Graphics/Mesh.h"
#include "Nyx/Graphics/Material.h"
#include "Nyx/graphics/Camera.h"
#include "Nyx/graphics/RenderPass.h"

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
			vertexBuffer->SetLayout({
				{ShaderDataType::Vec2, "a_Position"},
				{ShaderDataType::Vec2, "a_TextureCoords"}
			});
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

	class Renderer
	{
	public:
		static void Init();

		static void BeginRenderPass(Ref<RenderPass> renderPass);
		static void EndRenderPass();

		// Scene stuff
		static void BeginScene(Ref<Camera> camera);
		static void EndScene();

		static void SetEnvironment(Ref<EnvironmentMap> environmentMap, Ref<LightEnvironment> lightEnvironment);

		static void SubmitMesh(Ref<Mesh> mesh, glm::mat4 transform, Ref<Material> materialOverride);
		static void DrawFullscreenQuad(Ref<Material> material);
	private:
		static void FlushDrawList();

		static void InitRendereResourceFunctions();
		static void GenerateUniformBuffer(uint32_t& bufferID, uint32_t bindingPoint, uint32_t size);
		static void UploadUniformBuffer(uint32_t bufferID, uint32_t bindingPoint, uint32_t size, const void* data);
	};
}