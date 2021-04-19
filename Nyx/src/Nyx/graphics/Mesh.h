#pragma once
#include "Nyx/Graphics/Material.h"
#include "Nyx/Graphics/API/IndexBuffer.h"
#include "Nyx/Graphics/API/VertexArray.h"
#include "Nyx/Graphics/API/VertexBuffer.h"
#include "Nyx/Math/AABB.h"
#include "Nyx/Math/Triangle.h"

#include <assimp/scene.h>

namespace Nyx {

	struct Vertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec3 Tangent;
		glm::vec3 Binormal;
		glm::vec2 TextureCoords;
	};

	struct SubMesh 
	{
		uint32_t VertexOffset = 0;
		uint32_t IndexOffset = 0;
		uint32_t IndexCount = 0;
		uint32_t MaterialIndex = 0;
		uint32_t TriangleOffset = 0;
		uint32_t TriangleCount = 0;
		AABB BoundingBox = AABB();
		glm::mat4 Transform = glm::mat4(1);

		SubMesh() = default;
	};
	 
	class Mesh : public Asset
	{
	public:
		Mesh(const std::string& path);

	public:

		bool Reload(); // This function is not implemented as of now

		Ref<IndexBuffer> GetIndexBuffer() { return m_IndexBuffer; }
		Ref<VertexArray> GetVertexArray() { return m_VertexArray; }

		std::vector<SubMesh>& GetSubMeshs() { return m_SubMeshes; }
		const std::vector<Ref<Material>>& GetMaterials() const { return m_Materials; }
		const std::string& GetPath() const { return m_Path; }

	private:
		bool Load(const std::string& path);
		void LoadData(const aiScene* scene);
		void LoadMaterials(const aiScene* scene);
		Ref<Texture> LoadMaterialTexture(const std::string& path);
		void CalculateNodeTransforms(aiNode* node, const aiScene* scene, aiMatrix4x4 parentTransform);

	private:
		std::string m_Path;

		std::vector<SubMesh> m_SubMeshes;
		std::vector<Ref<Material>> m_Materials;
		std::vector<Triangle> m_Triangles;

		std::vector<Vertex> m_Vertices;
		std::vector<uint32_t> m_Indices;

		uint32_t m_BaseVertexPointer = 0;
		uint32_t m_BaseIndexPointer = 0;
		uint32_t m_BaseTrianglePointer = 0;

		Ref<IndexBuffer> m_IndexBuffer;
		Ref<VertexArray> m_VertexArray;
		Ref<VertexBuffer> m_VertexBuffer;

		Ref<AABB> m_BoundingBox;
	};
}