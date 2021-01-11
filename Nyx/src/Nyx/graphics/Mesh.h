#pragma once
#include "Nyx/Core/Core.h"
#include "Nyx/Graphics/API/IndexBuffer.h"
#include "Nyx/Graphics/API/VertexArray.h"
#include "Nyx/Graphics/API/VertexBuffer.h"
#include "Nyx/Graphics/Shader.h"
#include "nyx/Graphics/Material.h"
#include "Nyx/Math/AABB.h"
#include "Nyx/Math/Triangle.h"
#include "Nyx/Asset/Asset.h"

#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include <String>
#include <vector>
#include <unordered_map>

namespace Nyx {

	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec3 tangent;
		glm::vec3 binormal;
		glm::vec2 textureCoords;
	};

	struct SubMesh 
	{
		uint vertexOffset = 0;
		uint indexOffset = 0;
		uint indexCount = 0;
		uint materialIndex = 0;
		AABB boundingBox = AABB();
		std::vector<Triangle> triangles;
		glm::mat4 transform = glm::mat4(1);

		SubMesh()
		{

		}

		SubMesh(uint vertexOffset, uint indexOffset, uint indexCount)
			: vertexOffset(vertexOffset), indexOffset(indexOffset), indexCount(indexCount)
		{
		}
	};
	 
	class Mesh : public Asset
	{
	public:
		Mesh(const std::string& path);
		Mesh(Ref<IndexBuffer> indexBuffer, Ref<VertexBuffer> vertexBuffer, Ref<VertexArray> vertexArray);
		~Mesh();

	public:
		void Render(bool depthTesting);
		void DebugDrawBoundingBox(const glm::mat4& transform) const;

		void RenderImGuiNodeHierarchy(bool& open);

		void RenderImGuiVertexData();
		void RenderImGuiNodeTree(bool isOwnWindow) const;
		Ref<Shader> GetBaseShader() const { return m_BaseShader; }
		Ref<AABB> GetBoundingBox() { return m_BoundingBox; }
		bool Reload(const std::string& path);
		
		Ref<IndexBuffer> GetIndexBuffer() { return m_IndexBuffer; }
		Ref<VertexArray> GetVertexArray() { return m_VertexArray; }
		std::vector<SubMesh>& GetSubMeshs() { return m_SubMeshes; }

		const std::vector<Ref<Material>>& GetMaterials() const { return m_Materials; }

		const std::string& GetPath() const { return m_Path; }
	private:
		bool Load(const std::string& path);
		void processNode(aiNode* node, const aiScene* scene, aiMatrix4x4 parentTransform);
		SubMesh processMesh(aiMesh* mesh, const aiScene* scene);
		void ImGuiNodeHierarchy(aiNode* node, const glm::mat4& parentTransform = glm::mat4(1.0f), uint32_t level = 0);

		Ref<Texture> LoadMaterialTexture(const std::string& str);

		void DrawImGuiNode(aiNode* node) const;

		inline glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4 from)
		{
			glm::mat4 to;
			to[0][0] = (float)from.a1; to[0][1] = (float)from.b1;  to[0][2] = (float)from.c1; to[0][3] = (float)from.d1;
			to[1][0] = (float)from.a2; to[1][1] = (float)from.b2;  to[1][2] = (float)from.c2; to[1][3] = (float)from.d2;
			to[2][0] = (float)from.a3; to[2][1] = (float)from.b3;  to[2][2] = (float)from.c3; to[2][3] = (float)from.d3;
			to[3][0] = (float)from.a4; to[3][1] = (float)from.b4;  to[3][2] = (float)from.c4; to[3][3] = (float)from.d4;
			return to;
		}

	private:
		std::string m_Path;
		std::vector<SubMesh> m_SubMeshes;
		std::vector<Ref<Texture>> m_TexturesLoaded;
		std::unordered_map<std::string, Ref<Texture>> m_LoadedTextures;

		std::vector<Vertex> m_Vertices;
		std::vector<uint> m_Indices;

		Ref<IndexBuffer> m_IndexBuffer;
		Ref<VertexArray> m_VertexArray;
		Ref<VertexBuffer> m_VertexBuffer;

		Ref<Shader> m_BaseShader;

		uint m_BaseVertexPointer = 0;
		uint m_BaseIndexPointer = 0;

		aiScene* m_Scene;

		std::vector<Ref<Material>> m_Materials;

		Ref<AABB> m_BoundingBox;


		glm::vec3 m_bbMin = { FLT_MAX,  FLT_MAX,  FLT_MAX };
		glm::vec3 m_bbMax = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

		int m_VertexViewerEnd = 25;
		int m_VertexViewerStart = 0;

		bool m_ViewerShowPosition = true;
		bool m_ViewerShowNormal = true;
		bool m_ViewerShowTangent = true;
		bool m_ViewerShowBinormal = true;
		bool m_ViewerShowTextureCoords = true;
	};
}