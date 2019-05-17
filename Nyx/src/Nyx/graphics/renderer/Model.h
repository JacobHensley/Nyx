#pragma once
#include "Nyx/Common.h"
#include "Nyx/graphics/renderer/shaders/Shader.h"
#include "Nyx/graphics/renderer/Mesh.h"

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
	private:
		const String& m_Path;
		std::vector<Mesh> m_Meshes;

		String m_Directory;
		std::vector<Texture> m_TexturesLoaded;

		void Load();
		void processNode(aiNode* node, const aiScene* scene);
		Mesh processMesh(aiMesh* mesh, const aiScene *scene);
		std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, const String& typeName);
		uint TextureFromFile(const char* path, const String& directory);
	};

}