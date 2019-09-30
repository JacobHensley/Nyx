#include "NXpch.h"
#include "Mesh.h"
#include "glad/glad.h"
#include "Nyx/graphics/DebugRenderer.h"

namespace Nyx {

	Mesh::Mesh(const String& path)
		:	m_Path(path)
	{
		Load();
	}

	Mesh::~Mesh()
	{
	}

	void Mesh::Render()
	{
		m_VertexArray->Bind();
		m_IndexBuffer->Bind();
		glEnable(GL_BLEND);
		for (SubMesh& mesh : m_SubMeshes)
		{
			//renderer.submit(vertexarray)
			glDrawElementsBaseVertex(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, (void*)(mesh.indexOffset * sizeof(uint)), mesh.vertexOffset);
		}
	}

	void Mesh::DebugDrawBoundingBox(const glm::mat4& transform) const
	{
		DebugRenderer::DrawAABB(m_BoundingBox, transform);
	}

	void Mesh::Load()
	{
		// Read file via ASSIMP
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(m_Path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
		
		// Check for errors
		if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
			NX_CORE_ASSERT(false, importer.GetErrorString());

		// Process ASSIMP's root node recursively
		processNode(scene->mRootNode, scene);
		m_BoundingBox = AABB(m_bbMin, m_bbMax);

		m_VertexBuffer = new VertexBuffer(m_Vertices.data(), sizeof(Vertex) * m_Vertices.size());
		m_IndexBuffer = new IndexBuffer(m_Indices.data(), m_Indices.size());
		m_VertexArray = new VertexArray();
		m_VertexBuffer->SetLayout({
			{ShaderDataType::Vec3, "a_Position"},
			{ShaderDataType::Vec3, "a_Normal"},
			{ShaderDataType::Vec3, "a_Tangent"},
			{ShaderDataType::Vec3, "a_Binormal"},
			{ShaderDataType::Vec2, "a_TextureCoords"}
		});

		m_VertexArray->PushVertexBuffer(m_VertexBuffer);

	}

	// Processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
	void Mesh::processNode(aiNode* node, const aiScene* scene)
	{
		// Process each mesh located at the current node
		for (uint i = 0; i < node->mNumMeshes; i++)
		{
			// The node object only contains indices to index the actual objects in the scene.
			// The scene contains all the data, node is just to keep stuff organized (like relations between nodes).
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

			m_SubMeshes.push_back(processMesh(mesh, scene));
		}

		// After we've processed all of the meshes (if any) we then recursively process each of the children nodes
		for (uint i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene);
		}
	}

	SubMesh Mesh::processMesh(aiMesh* mesh, const aiScene* scene)
	{
		// Data to fill
		std::vector<Texture> textures;

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

			m_bbMin = glm::min(vector, m_bbMin);
			m_bbMax = glm::max(vector, m_bbMax);

			// Normals
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.normal = vector;

			// Bi-Tangents
			vector.x = mesh->mBitangents[i].x;
			vector.y = mesh->mBitangents[i].y;
			vector.z = mesh->mBitangents[i].z;
			vertex.binormal = vector;

			// Tangents
			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.tangent = vector;

			// Texture Coordinates
			if (mesh->HasTextureCoords(0)) // Does the mesh contain texture coordinates?
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

			m_Vertices.push_back(vertex);
		}

		// Now walk through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
		for (uint i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			// Retrieve all indices of the face and store them in the indices vector
			for (uint j = 0; j < face.mNumIndices; j++)
			{
				m_Indices.push_back(face.mIndices[j]);
			}
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
			std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

			// 2. Specular maps
			std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

			// 2. Normal maps
			std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_normal");
			textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		}

		uint baseVertex = m_BaseVertexPointer;
		m_BaseVertexPointer += mesh->mNumVertices;
		uint baseIndex = m_BaseIndexPointer;
		uint indexCount = mesh->mNumFaces * 3;
		m_BaseIndexPointer += indexCount;
		
		return SubMesh(baseVertex, baseIndex, indexCount);
	}

	// Checks all material textures of a given type and loads the textures if they're not loaded yet.
	// The required info is returned as a Texture struct.
	std::vector<Texture> Mesh::loadMaterialTextures(aiMaterial* mat, aiTextureType type, const String& typeName)
	{
		std::vector<Texture> textures;

		for (uint i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);

			// Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
			bool skip = false;

			for (uint j = 0; j < m_TexturesLoaded.size(); j++)
			{
				if (m_TexturesLoaded[j].GetPath() == str.C_Str())
				{
					textures.push_back(m_TexturesLoaded[j]);
					skip = true; // A texture with the same filepath has already been loaded, continue to next one. (optimization)

					break;
				}
			}

			if (!skip)
			{
				// If texture hasn't been loaded already, load it
			//	String path = str.C_Str();
			//	Texture texture = Texture(str.C_Str());

		//		m_TexturesLoaded.push_back(texture);  // Store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
			}
		}

		return textures;
	}

}