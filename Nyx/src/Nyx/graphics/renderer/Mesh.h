#pragma once
#include "Nyx/Common.h"
#include "NXpch.h"
#include "Nyx/graphics/renderer/API/IndexBuffer.h"
#include "Nyx/graphics/renderer/API/VertexArray.h"
#include "Nyx/graphics/renderer/API/VertexBuffer.h"
#include "Nyx/graphics/renderer/shaders/Shader.h"
#include "Nyx/math/AABB.h"
#include "glm/glm.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

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
		uint vertexOffset;
		uint indexOffset;
		uint indexCount;
		AABB boundingBox;

		SubMesh(uint vertexOffset, uint indexOffset, uint indexCount)
			: vertexOffset(vertexOffset), indexOffset(indexOffset), indexCount(indexCount)
		{

		}
	};

	class Mesh
	{
	public:
		Mesh(const String& path);
		Mesh(IndexBuffer* indexBuffer, VertexBuffer* vertexBuffer, VertexArray* vertexArray);
		~Mesh();

		void Render(bool depthTesting);
		void DebugDrawBoundingBox(const glm::mat4& transform) const;
		void RenderImGuiVertexData();
		void RenderImGuiNodeTree(bool isOwnWindow) const;

		bool Reload(const String& path);
	private:
		bool Load(const String& path);
		void processNode(aiNode* node, const aiScene* scene);
		SubMesh processMesh(aiMesh* mesh, const aiScene* scene);
		std::vector<Texture> Mesh::loadMaterialTextures(aiMaterial* mat, aiTextureType type, const String& typeName);

		void DrawImGuiNode(aiNode* node) const;
		String m_Path;
		std::vector<SubMesh> m_SubMeshes;
		std::vector<Texture> m_TexturesLoaded;
		AABB m_BoundingBox;
		aiScene* m_Scene;
		std::vector<Vertex> m_Vertices;
		std::vector<uint> m_Indices;
		uint m_BaseVertexPointer = 0;
		uint m_BaseIndexPointer = 0;

		glm::vec3 m_bbMin = { FLT_MAX,  FLT_MAX,  FLT_MAX };
		glm::vec3 m_bbMax = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

		int m_VertexViewerEnd = 25;
		int m_VertexViewerStart = 0;
		bool m_ViewerShowPosition = true;
		bool m_ViewerShowNormal = true;
		bool m_ViewerShowTangent = true;
		bool m_ViewerShowBinormal = true;
		bool m_ViewerShowTextureCoords = true;


		IndexBuffer* m_IndexBuffer;
		VertexArray* m_VertexArray;
		VertexBuffer* m_VertexBuffer;
	};
}