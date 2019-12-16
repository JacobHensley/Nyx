#include "NXpch.h"
#include "MeshFactory.h"
#include "Nyx/graphics/renderer/Mesh.h"

namespace Nyx {

	MeshFactory* MeshFactory::s_Instance = new MeshFactory();

	Mesh* MeshFactory::GenQuad(float x, float y, float z, float width, float height)
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

		VertexBuffer* vertexBuffer = new VertexBuffer(&vertices[0], (int)(sizeof(QuadVertex) * vertices.size()));

		BufferLayout layout = {
			{ShaderDataType::Vec3, "a_Position"},
			{ShaderDataType::Vec2, "a_TexCoord"}
		};

		vertexBuffer->SetLayout(layout);

		IndexBuffer* indexBuffer = new IndexBuffer(&indices[0], (uint)indices.size());

		VertexArray* vertexArray = new VertexArray();
		vertexArray->PushVertexBuffer(vertexBuffer);

		return new Mesh(indexBuffer, vertexBuffer, vertexArray);
	}

}