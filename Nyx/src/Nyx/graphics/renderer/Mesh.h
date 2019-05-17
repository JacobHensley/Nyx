#pragma once
#include "Nyx/graphics/renderer/API/IndexBuffer.h"
#include "Nyx/graphics/renderer/API/VertexArray.h"
#include "Nyx/graphics/renderer/API/VertexBuffer.h"
#include "Nyx/graphics/renderer/shaders/Shader.h"

namespace Nyx {

	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 textureCoords;
	};

	struct Texture
	{
		uint id;
		String type;
		String path;
	};

	class Mesh
	{
	public:
		Mesh(std::vector<Vertex>& vertices, std::vector<uint>& indices, std::vector<Texture>& textures);
		~Mesh();

		void Render(const Shader& shader);

	private:
		void Init();

		std::vector<Vertex> m_Vertices;
		std::vector<uint> m_Indices;
		std::vector<Texture> m_Textures;

		IndexBuffer* m_IndexBuffer;
		VertexArray* m_VertexArray;
		VertexBuffer* m_VertexBuffer;
	};

}