#include "NXpch.h"
#include "Mesh.h"
#include "glad/glad.h"
#include "Nyx/graphics/DebugRenderer.h"
#include "imgui/imgui.h"

namespace Nyx {

	Mesh::Mesh(const String& path)
		:	m_Path(path)
	{
		NX_CORE_ASSERT(Load(path), "Failed to load model");
	}

	Mesh::Mesh(IndexBuffer* indexBuffer, VertexBuffer* vertexBuffer, VertexArray* vertexArray)
		:	m_IndexBuffer(indexBuffer), m_VertexBuffer(vertexBuffer), m_VertexArray(vertexArray), m_Path("GenMesh")
	{
		m_SubMeshes.push_back(SubMesh(0, 0, indexBuffer->GetBufferID()));
	}

	Mesh::~Mesh()
	{
	}

	void Mesh::Render(bool depthTesting)
	{
		if (!depthTesting)
			glDisable(GL_DEPTH_TEST);

		m_VertexArray->Bind();
		m_IndexBuffer->Bind();
		glEnable(GL_BLEND);

		for (SubMesh& mesh : m_SubMeshes)
		{
			//renderer.submit(vertexarray)
			glDrawElementsBaseVertex(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, (void*)(mesh.indexOffset * sizeof(uint)), mesh.vertexOffset);
		}

		if (!depthTesting)
			glEnable(GL_DEPTH_TEST);
	}

	void Mesh::DebugDrawBoundingBox(const glm::mat4& transform) const
	{
		DebugRenderer::DrawAABB(m_BoundingBox, transform);
	}

	void Mesh::RenderImGuiVertexData()
	{
		if (m_Vertices.size() == 0)
			NX_CORE_ASSERT(false, "Failed to display vertex data");

			aiNode* rootNode = m_Scene->mRootNode;
			String id = "##" + std::to_string((int)rootNode);
			String name = rootNode->mName.C_Str() + id;

		ImGui::Begin(name.c_str());
		
		ImGui::Checkbox("Show Position", &m_ViewerShowPosition);
		ImGui::Checkbox("Show Normal", &m_ViewerShowNormal);
		ImGui::Checkbox("Show Tangent", &m_ViewerShowTangent);
		ImGui::Checkbox("Show Binormal", &m_ViewerShowBinormal);
		ImGui::Checkbox("Show TextureCoords", &m_ViewerShowTextureCoords);

		ImGui::Text("Min X: %f.1, Y: %f.1, Z: %f.1,", m_bbMin.x, m_bbMin.y, m_bbMin.z);
		ImGui::Text("Max X: %f.1, Y: %f.1, Z: %f.1,", m_bbMax.x, m_bbMax.y, m_bbMax.z);

		ImGui::Text("Number of Vertices: %i", m_Vertices.size());

		if (ImGui::ArrowButton("Previous Page", ImGuiDir_Left)) 
		{
			m_VertexViewerStart -= 25;
			m_VertexViewerEnd -= 25;
		}
		ImGui::SameLine();
		if (ImGui::ArrowButton("Next Page", ImGuiDir_Right))
		{
			m_VertexViewerStart += 25;
			m_VertexViewerEnd += 25;
		}

		if (m_VertexViewerEnd >= m_Vertices.size())
			m_VertexViewerStart = m_Vertices.size();

		if (m_VertexViewerEnd < 25)
			m_VertexViewerEnd = 25;

		if (m_VertexViewerStart < 0)
			m_VertexViewerStart = 0;

		for (int i = m_VertexViewerStart; i < m_VertexViewerEnd; i++)
		{
			Vertex vertex = m_Vertices[i];
			ImGui::Text("Vertex Number #%i", i);
			if (m_ViewerShowPosition)
				ImGui::Text("Position:      X: %.2f, Y: %.2f, Z: %.2f", vertex.position.x, vertex.position.y, vertex.position.z);
			if (m_ViewerShowNormal)
				ImGui::Text("Normal:        X: %.2f, Y: %.2f, Z: %.2f", vertex.normal.x, vertex.normal.y, vertex.normal.z);
			if (m_ViewerShowTangent)
				ImGui::Text("Tangent:       X: %.2f, Y: %.2f, Z: %.2f", vertex.tangent.x, vertex.tangent.y, vertex.tangent.z);
			if (m_ViewerShowBinormal)
				ImGui::Text("Binormal:      X: %.2f, Y: %.2f, Z: %.2f", vertex.binormal.x, vertex.binormal.y, vertex.binormal.z);
			if (m_ViewerShowTextureCoords)
				ImGui::Text("TextureCoords: X: %.2f, Y: %.2f", vertex.textureCoords.x, vertex.textureCoords.y);
			ImGui::Separator();
		}
		ImGui::End();
	}

	void Mesh::RenderImGuiNodeTree(bool isOwnWindow) const
	{
		if (m_Path == "GenMesh")
			NX_CORE_ASSERT(false, "Cannot Render node graph of generated mesh");

		aiNode* rootNode = m_Scene->mRootNode;
		String id = "##" + std::to_string((int)rootNode);
		String name = rootNode->mName.C_Str() + id;
		String windowName = "Node Graph" + id;
		if (isOwnWindow)
			ImGui::Begin(windowName.c_str());

		if (ImGui::TreeNode(name.c_str()))
		{
			for (uint i = 0; i < rootNode->mNumChildren; i++) 
			{
				if (ImGui::TreeNode((void*)(intptr_t)i, rootNode->mChildren[i]->mName.C_Str(), i))
				{	
					//Node info here
					DrawImGuiNode(rootNode->mChildren[i]);
					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}

		if (isOwnWindow)
			ImGui::End();
	}

	bool Mesh::Reload(const String& path)
	{
		m_Vertices.clear();
		m_Indices.clear();
		if (!Load(path)) 
		{
			Load(m_Path);
			NX_CORE_WARN("Failed to reload model: {0}, aborting", path);
			return false;
		}

		m_Path = path;
		return true;
	}

	void Mesh::DrawImGuiNode(aiNode* node) const
	{
		for (uint i = 0; i < node->mNumChildren; i++) 
		{
			if (ImGui::TreeNode((void*)(intptr_t)i, node->mChildren[i]->mName.C_Str(), i))
			{
				//Node info here
				DrawImGuiNode(node->mChildren[i]);
				ImGui::TreePop();
			}
		}
	}

	bool Mesh::Load(const String& path)
	{
		// Read file via ASSIMP
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

		// Check for errors
		if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
			return false;
			
		m_Scene = importer.GetOrphanedScene();

		// Process ASSIMP's root node recursively

		processNode(scene->mRootNode, scene);
		m_BoundingBox = AABB(m_bbMin, m_bbMax);

		m_VertexBuffer = new VertexBuffer(m_Vertices.data(), int(sizeof(Vertex) * m_Vertices.size()));
		m_IndexBuffer = new IndexBuffer(m_Indices.data(), (int)m_Indices.size());
		m_VertexArray = new VertexArray();
		m_VertexBuffer->SetLayout({
			{ShaderDataType::Vec3, "a_Position"},
			{ShaderDataType::Vec3, "a_Normal"},
			{ShaderDataType::Vec3, "a_Tangent"},
			{ShaderDataType::Vec3, "a_Binormal"},
			{ShaderDataType::Vec2, "a_TextureCoords"}
			});

		m_VertexArray->PushVertexBuffer(m_VertexBuffer);
		return true;
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