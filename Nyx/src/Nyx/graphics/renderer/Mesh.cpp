#include "NXpch.h"
#include "Mesh.h"
#include "Nyx/graphics/renderer/API/BufferLayout.h"
#include "glad/glad.h"

namespace Nyx {

	Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<uint>& indices, std::vector<Texture>& textures)
		: m_Vertices(vertices), m_Indices(indices), m_Textures(textures)
	{
		Init();
	}

	Mesh::~Mesh()
	{
	//	delete m_IndexBuffer;
	//	delete m_VertexArray;
	//	delete m_VertexBuffer;
	}

	void Mesh::Render(const Shader& shader)
	{
		glBindVertexArray(VAO);
	//	m_VertexArray->Bind();
	//	m_IndexBuffer->Bind();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//	m_IndexBuffer->Draw();
		glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, 0);
	//	m_IndexBuffer->Unbind();
	//	m_VertexArray->Unbind();
	}

	void Mesh::Init()
	{
	/*	m_IndexBuffer = new IndexBuffer();
		m_VertexArray = new VertexArray();
		m_VertexBuffer = new VertexBuffer();

		m_VertexBuffer->SetData((float*)&m_Vertices[0], m_Vertices.size() * sizeof(Vertex));
		m_IndexBuffer->SetData((uint*)m_Indices[0], m_Indices.size());

		BufferLayout layout;
		layout.Push<glm::vec3>("Position");
		layout.Push<glm::vec3>("Normal");
		layout.Push<glm::vec2>("TextureCoords");

		m_VertexBuffer->SetLayout(layout); 

		m_VertexArray->PushVertexBuffer(m_VertexBuffer); */
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(Vertex), &m_Vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(unsigned int),
			&m_Indices[0], GL_STATIC_DRAW);

		// vertex positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		// vertex normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
		// vertex texture coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, textureCoords));

		glBindVertexArray(0);
	}

}