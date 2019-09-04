#pragma once
#include "Nyx/graphics/renderer/API/IndexBuffer.h"
#include "Nyx/graphics/renderer/API/VertexArray.h"
#include "Nyx/graphics/renderer/API/VertexBuffer.h"
#include "Nyx/graphics/renderer/shaders/Shader.h"
#include "Nyx/math/AABB.h"

namespace Nyx {

	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec3 tangent;
		glm::vec3 binormal;
		glm::vec2 textureCoords;
	};

	struct MeshTexture
	{
		uint id;
		String type;
		String path;
	};

	class Mesh
	{
	public:
		Mesh(std::vector<Vertex>& vertices, std::vector<uint>& indices, std::vector<MeshTexture>& textures, std::vector<glm::vec3>& tangents, std::vector<glm::vec3>& binormals);
		~Mesh();

		void Render(const Shader& shader);

		const AABB& GetBoundingBox() const { return m_BoundingBox; }
		void SetBoundingBox(const AABB& boundingBox) { m_BoundingBox = boundingBox; }
	private:
		void Init();

		std::vector<Vertex> m_Vertices;
		std::vector<uint> m_Indices;
		std::vector<MeshTexture> m_Textures;

		std::vector<glm::vec3> m_Tangents;
		std::vector<glm::vec3> m_BiNormals;

		IndexBuffer* m_IndexBuffer;
		VertexArray* m_VertexArray;
		VertexBuffer* m_VertexBuffer;

		AABB m_BoundingBox;
	};

}