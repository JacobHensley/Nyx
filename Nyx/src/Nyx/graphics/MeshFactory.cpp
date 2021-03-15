#include "NXpch.h"
#include "MeshFactory.h"
#include "Nyx/Graphics/Mesh.h"
#include "Nyx/Graphics/PBRMaterial.h"
#include "Nyx/Graphics/SceneRenderer.h"

namespace Nyx {

	MeshFactory* MeshFactory::s_Instance = new MeshFactory();

	Ref<Mesh> MeshFactory::GenQuad(float x, float y, float z, float width, float height)
	{
		struct QuadVertex
		{
			glm::vec3 Position;
			glm::vec2 TexCoord;
		};

		std::array<QuadVertex, 4> vertices;

		vertices[0].Position = { x, y, z };
		vertices[0].TexCoord = { 0.0f, 0.0f };

		vertices[1].Position = { x + width, y, z };
		vertices[1].TexCoord = { 1.0f, 0.0f };

		vertices[2].Position = { x + width, y + height, z };
		vertices[2].TexCoord = { 1.0f, 1.0f };

		vertices[3].Position = { x, y + height, z };
		vertices[3].TexCoord = { 0.0f, 1.0f };

		std::array<uint32_t, 6> indices = { 0, 1, 2, 2, 3, 0 };

		Ref<VertexBuffer> vertexBuffer = CreateRef<VertexBuffer>(&vertices[0], (int)(sizeof(QuadVertex) * vertices.size()));

		BufferLayout layout = {
			{ShaderDataType::Vec3, "a_Position"},
			{ShaderDataType::Vec2, "a_TexCoord"}
		};

		vertexBuffer->SetLayout(layout);

		Ref<IndexBuffer> indexBuffer = CreateRef<IndexBuffer>(&indices[0], (uint)indices.size());
		Ref<VertexArray> vertexArray = CreateRef<VertexArray>();
		vertexArray->PushVertexBuffer(vertexBuffer);

		Ref<Mesh> mesh = CreateRef<Mesh>(indexBuffer, vertexBuffer, vertexArray);
		mesh->GetMaterials().push_back(CreateRef<PBRMaterial>(SceneRenderer::GetPBRShader()));
		return mesh;
	}

}