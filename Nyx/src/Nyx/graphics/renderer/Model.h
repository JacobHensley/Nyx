#pragma once
#include "Nyx/Common.h"
#include "Nyx/graphics/renderer/Mesh.h"
#include "Nyx/graphics/renderer/shaders/Shader.h"


#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Nyx {

	class Model
	{
	public:
		Model(const String& path);
		~Model();

		void Render(const Shader& shader);

		const AABB& GetBoundingBox() const { return m_BoundingBox; }

		void DebugDrawBoundingBox(const glm::mat4& transform) const;
	private:
		const String& m_Path;
		std::vector<Mesh> m_Meshes;

		String m_Directory;
		std::vector<MeshTexture> m_TexturesLoaded;

		void Load();
		void processNode(aiNode* node, const aiScene* scene);
		Mesh processMesh(aiMesh* mesh, const aiScene *scene);
		std::vector<MeshTexture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, const String& typeName);
	private:
		AABB m_BoundingBox;
	};

}