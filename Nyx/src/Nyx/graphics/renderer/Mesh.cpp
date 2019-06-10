#include "NXpch.h"
#include "Mesh.h"
#include "Nyx/graphics/renderer/API/BufferLayout.h"

namespace Nyx {

	Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<uint>& indices, std::vector<MeshTexture>& textures, std::vector<glm::vec3>& tangents, std::vector<glm::vec3>& binormals)
		: m_Vertices(vertices), m_Indices(indices), m_Textures(textures), m_Tangents(tangents), m_BiNormals(binormals)
	{
		Init();
	}

	Mesh::~Mesh()
	{
	}

	void Mesh::Render(const Shader& shader)
	{
		m_VertexArray->Bind();
		m_IndexBuffer->Bind();
		m_IndexBuffer->Draw();;
	}

	void Mesh::Init()
	{
		m_VertexArray = new VertexArray();
		m_VertexBuffer = new VertexBuffer();
		m_IndexBuffer = new IndexBuffer();

		m_VertexArray->Bind();

		m_VertexBuffer->Bind();
		m_VertexBuffer->SetData((float*)&m_Vertices[0], m_Vertices.size() * sizeof(Vertex));

		m_IndexBuffer->Bind();
		m_IndexBuffer->SetData(&m_Indices[0], m_Indices.size());

		BufferLayout layout;
		layout.Push<glm::vec3>("position");
		layout.Push<glm::vec3>("normal");
		layout.Push<glm::vec3>("tangent");
		layout.Push<glm::vec3>("binormals");
		layout.Push<glm::vec2>("textureCoords");

		m_VertexBuffer->SetLayout(layout);

		m_VertexArray->PushVertexBuffer(m_VertexBuffer);

		m_VertexArray->Unbind();
	}

}