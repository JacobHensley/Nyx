#include "NXpch.h"
#include "Model.h"
#include "Nyx/graphics/renderer/API/Texture.h"

namespace Nyx {

	Model::Model(const String& path)
		:	m_Path(path)
	{
		Load();
	}

	Model::~Model()
	{
	}

	void Model::Render(const Shader& shader) 
	{
		for (int i = 0;i < m_Meshes.size();i++)
		{
			m_Meshes[i].Render(shader);
		}
	}

	// Loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
	void Model::Load()
	{
		// Read file via ASSIMP
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(m_Path, aiProcess_Triangulate | aiProcess_FlipUVs);

		// Check for errors
		if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
			NX_CORE_ASSERT(false, importer.GetErrorString());

		// Retrieve the directory path of the filepath
		m_Directory = m_Path.substr(0, m_Path.find_last_of('/'));

		// Process ASSIMP's root node recursively
		processNode(scene->mRootNode, scene);
	}

	// Processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
	void Model::processNode(aiNode* node, const aiScene* scene)
	{
		// Process each mesh located at the current node
		for (uint i = 0; i < node->mNumMeshes; i++)
		{
			// The node object only contains indices to index the actual objects in the scene.
			// The scene contains all the data, node is just to keep stuff organized (like relations between nodes).
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

			m_Meshes.push_back(processMesh(mesh, scene));
		}

		// After we've processed all of the meshes (if any) we then recursively process each of the children nodes
		for (uint i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene);
		}
	}

	Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
	{
		// Data to fill
		std::vector<Vertex> vertices;
		std::vector<uint> indices;
		std::vector<MeshTexture> textures;
		std::vector<glm::vec3> tangents;
		std::vector<glm::vec3> binormals;

		// Walk through each of the mesh's vertices
		for (uint i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			glm::vec3 vector; // We declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.

			// Positions
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex.position = vector;

			// Normals
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.normal = vector;

			glm::vec3 binormal;
			binormal.x = mesh->mBitangents[i].x;
			binormal.y = mesh->mBitangents[i].y;
			binormal.z = mesh->mBitangents[i].z;
			vertex.binormal = binormal;

			glm::vec3 tangent;
			tangent.x = mesh->mTangents[i].x;
			tangent.y = mesh->mTangents[i].y;
			tangent.z = mesh->mTangents[i].z;
			vertex.tangent = tangent;

			// Texture Coordinates
			if (mesh->mTextureCoords[0]) // Does the mesh contain texture coordinates?
			{
				glm::vec2 vec;
				// A vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
				// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.textureCoords = vec;
			}
			else
			{
				vertex.textureCoords = glm::vec2(0.0f, 0.0f);
			}

			vertices.push_back(vertex);
		}

		// Now walk through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
		for (uint i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			// Retrieve all indices of the face and store them in the indices vector
			for (uint j = 0; j < face.mNumIndices; j++)
			{
				indices.push_back(face.mIndices[j]);
			}
		}

		for (uint i = 0; i < mesh->mNumVertices; i++)
		{
			glm::vec3 tangent;
			tangent.x = mesh->mTangents[i].x;
			tangent.y = mesh->mTangents[i].y;
			tangent.z = mesh->mTangents[i].z;
			tangents.push_back(tangent);
		}

		for (uint i = 0; i < mesh->mNumVertices; i++)
		{
			glm::vec3 binormal;
			binormal.x = mesh->mBitangents[i].x;
			binormal.y = mesh->mBitangents[i].y;
			binormal.z = mesh->mBitangents[i].z;
			binormals.push_back(binormal);
		}

		// Process materials
		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			// We assume a convention for sampler names in the shaders. Each diffuse texture should be named
			// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
			// Same applies to other texture as the following list summarizes:
			// Diffuse: texture_diffuseN
			// Specular: texture_specularN
			// Normal: texture_normalN

			// 1. Diffuse maps
			std::vector<MeshTexture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

			// 2. Specular maps
			std::vector<MeshTexture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		}

		// Return a mesh object created from the extracted mesh data
		return Mesh(vertices, indices, textures, tangents, binormals);
	}

	// Checks all material textures of a given type and loads the textures if they're not loaded yet.
	// The required info is returned as a Texture struct.
	std::vector<MeshTexture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, const String& typeName)
	{
		std::vector<MeshTexture> textures;

		for (uint i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);

			// Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
			bool skip = false;

			for (uint j = 0; j < m_TexturesLoaded.size(); j++)
			{
				if (m_TexturesLoaded[j].path == str.C_Str())
				{
					textures.push_back(m_TexturesLoaded[j]);
					skip = true; // A texture with the same filepath has already been loaded, continue to next one. (optimization)

					break;
				}
			}

			if (!skip)
			{   
				// If texture hasn't been loaded already, load it
				MeshTexture texture;
				texture.id = Texture(str.C_Str()).GetTextureID();
				texture.type = typeName;
				texture.path = str.C_Str();
				textures.push_back(texture);

				m_TexturesLoaded.push_back(texture);  // Store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
			}
		}

		return textures;
	}

}