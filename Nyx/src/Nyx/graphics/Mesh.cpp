#include "NXpch.h"
#include "Mesh.h"
#include "Nyx/Graphics/Debug/DebugRenderer.h"
#include "Nyx/Graphics/SceneRenderer.h"
#include "Nyx/Graphics/PBRMaterial.h"

#include <assimp/pbrmaterial.h>
#include <assimp/postprocess.h>

#include <imgui/imgui.h>
#include <glm/gtx/matrix_decompose.hpp>
#include <glad/glad.h>

#include <filesystem>

namespace Nyx {

	Mesh::Mesh(const std::string& path)
		:	m_Path(path)
	{
		NX_CORE_ASSERT(Load(path), "Failed to load model");	
		NX_CORE_INFO("Created Mesh at Path: {0}", m_Path);
	}

	Mesh::Mesh(Ref<IndexBuffer> indexBuffer, Ref<VertexBuffer> vertexBuffer, Ref<VertexArray> vertexArray)
		:	m_IndexBuffer(indexBuffer), m_VertexBuffer(vertexBuffer), m_VertexArray(vertexArray), m_Path("GenMesh")
	{
		m_SubMeshes.push_back(SubMesh(0, 0, indexBuffer->GetCount()));
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
			glDrawElementsBaseVertex(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, (void*)(mesh.indexOffset * sizeof(uint)), mesh.vertexOffset);
		}

		if (!depthTesting)
			glEnable(GL_DEPTH_TEST);
	}

	void Mesh::DebugDrawBoundingBox(const glm::mat4& transform) const
	{	
		for (auto& mesh : m_SubMeshes)
		{
			DebugRenderer::DrawAABB(mesh.boundingBox, transform * mesh.transform);
		}
	}

	void Mesh::RenderImGuiNodeHierarchy(bool& open)
	{
		ImGui::Begin("Mesh Hierarchy", &open);
		ImGuiNodeHierarchy(m_Scene->mRootNode);
		ImGui::End();
	}

	void Mesh::RenderImGuiVertexData()
	{
		if (m_Vertices.size() == 0)
		{
			NX_CORE_ASSERT(false, "Failed to display vertex data");
		}

		aiNode* rootNode = m_Scene->mRootNode;
		std::string id = "##" + std::to_string((UINT64)rootNode);
		std::string name = rootNode->mName.C_Str() + id;

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
		{
			m_VertexViewerStart = (int)m_Vertices.size();
		}

		if (m_VertexViewerEnd < 25)
		{
			m_VertexViewerEnd = 25;
		}

		if (m_VertexViewerStart < 0)
		{
			m_VertexViewerStart = 0;
		}

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
		{
			NX_CORE_ASSERT(false, "Cannot Render node graph of generated mesh");
		}

		aiNode* rootNode = m_Scene->mRootNode;
		std::string id = "##" + std::to_string((UINT64)rootNode);
		std::string name = rootNode->mName.C_Str() + id;
		std::string windowName = "Node Graph" + id;

		if (isOwnWindow)
		{
			ImGui::Begin(windowName.c_str());
		}

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
		{
			ImGui::End();
		}
	}

	static std::tuple<glm::vec3, glm::quat, glm::vec3> GetTransformDecomposition(const glm::mat4& transform)
	{
		glm::vec3 scale, translation, skew;
		glm::vec4 perspective;
		glm::quat orientation;
		glm::decompose(transform, scale, orientation, translation, skew, perspective);

		return { translation, orientation, scale };
	}

	void Mesh::ImGuiNodeHierarchy(aiNode* node, const glm::mat4& parentTransform, uint32_t level)
	{
		glm::mat4 localTransform = aiMatrix4x4ToGlm(node->mTransformation);
		glm::mat4 transform = parentTransform * localTransform;
		for (uint32_t i = 0; i < node->mNumMeshes; i++)
		{
			uint32_t mesh = node->mMeshes[i];
			m_SubMeshes[mesh].transform = transform;
		}

		std::string treenodeName = std::string(node->mName.C_Str()) + " (" + m_Path + ")";
		if (ImGui::TreeNode(treenodeName.c_str()))
		{
			{
				auto [translation, rotation, scale] = GetTransformDecomposition(transform);
				ImGui::Text("World Transform");
				ImGui::Text("  Translation: %.2f, %.2f, %.2f", translation.x, translation.y, translation.z);
				ImGui::Text("  Scale: %.2f, %.2f, %.2f", scale.x, scale.y, scale.z);
			}
			{
				auto [translation, rotation, scale] = GetTransformDecomposition(localTransform);
				ImGui::Text("Local Transform");
				ImGui::Text("  Translation: %.2f, %.2f, %.2f", translation.x, translation.y, translation.z);
				ImGui::Text("  Scale: %.2f, %.2f, %.2f", scale.x, scale.y, scale.z);
			}

			for (uint32_t i = 0; i < node->mNumChildren; i++)
				ImGuiNodeHierarchy(node->mChildren[i], transform, level + 1);

			ImGui::TreePop();
		}
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

	bool Mesh::Reload(const std::string& path)
	{
		m_Vertices.clear();
		m_Indices.clear();
		m_SubMeshes.clear();

		m_BaseIndexPointer = 0;
		m_BaseVertexPointer = 0;

		if (!Load(path)) 
		{
			Load(m_Path);
			NX_CORE_WARN("Failed to reload model: {0}, aborting", path);

			return false;
		}

		m_Path = path;
		return true;
	}

	bool Mesh::Load(const std::string& path)
	{
		// Read file via ASSIMP
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

		// Check for errors
		if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			NX_CORE_ERROR("Error {0}", importer.GetErrorString());
			return false;
		}
			
		m_Scene = importer.GetOrphanedScene();

		m_BaseShader = SceneRenderer::GetPBRShader();

		// Process ASSIMP's root node recursively
		processNode(scene->mRootNode, scene, scene->mRootNode->mTransformation);
		
	//	m_BoundingBox = CreateRef<AABB>(m_bbMin, m_bbMax);

		m_VertexBuffer = CreateRef<VertexBuffer>(m_Vertices.data(), int(sizeof(Vertex) * m_Vertices.size()));
		m_IndexBuffer = CreateRef<IndexBuffer>(m_Indices.data(), (int)m_Indices.size());
		m_VertexArray = CreateRef<VertexArray>();
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
	void Mesh::processNode(aiNode* node, const aiScene* scene, aiMatrix4x4 parentTransform)
	{
		aiMatrix4x4 transform = parentTransform * node->mTransformation;

		// Process each mesh located at the current node
		for (uint i = 0; i < node->mNumMeshes; i++)
		{
			// The node object only contains indices to index the actual objects in the scene.
			// The scene contains all the data, node is just to keep stuff organized (like relations between nodes).
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			SubMesh sub = processMesh(mesh, scene);
			sub.transform = aiMatrix4x4ToGlm(transform);
			m_SubMeshes.push_back(sub);
		}

		
		// After we've processed all of the meshes (if any) we then recursively process each of the children nodes
		for (uint i = 0; i < node->mNumChildren; i++)
		{
		//	node->mChildren[i]->mTransformation *= node->mTransformation;
			processNode(node->mChildren[i], scene, transform);
		}
	}

	SubMesh Mesh::processMesh(aiMesh* mesh, const aiScene* scene)
	{
		// Data to fill
		std::vector<Ref<Texture>> textures;

		SubMesh submesh;

		AABB boundingBox;
		boundingBox.Min = glm::vec3(FLT_MAX);
		boundingBox.Max = glm::vec3(-FLT_MAX);

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

			boundingBox.Min.x = glm::min(vector.x, boundingBox.Min.x);
			boundingBox.Min.y = glm::min(vector.y, boundingBox.Min.y);
			boundingBox.Min.z = glm::min(vector.z, boundingBox.Min.z);
			
			boundingBox.Max.x = glm::max(vector.x, boundingBox.Max.x);
			boundingBox.Max.y = glm::max(vector.y, boundingBox.Max.y);
			boundingBox.Max.z = glm::max(vector.z, boundingBox.Max.z);

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
			Triangle triangle;
			for (uint j = 0; j < face.mNumIndices; j++)
			{	
				triangle.Points[j].x = mesh->mVertices[face.mIndices[j]].x;
				triangle.Points[j].y = mesh->mVertices[face.mIndices[j]].y;
				triangle.Points[j].z = mesh->mVertices[face.mIndices[j]].z;
				m_Indices.push_back(face.mIndices[j]);
			}
			submesh.triangles.push_back(triangle);
		}

		if (scene->HasMaterials())
		{
			m_Materials.resize(scene->mNumMaterials);
			for (uint32_t i = 0; i < scene->mNumMaterials; i++)
			{
				auto aiMaterial = scene->mMaterials[i];
				auto aiMaterialName = aiMaterial->GetName();

			//	auto material = CreateRef<PBRMaterial>(m_BaseShader);
			//	m_Materials[i] = material;

				Ref<Texture> albedoMap, normalMap, roughnessMap, metalnessMap;

				for (uint32_t i = 0; i < aiMaterial->mNumProperties; i++)
				{
					auto prop = aiMaterial->mProperties[i];
					if (prop->mType == aiPTI_String)
					{
						uint32_t strLength = *(uint32_t*)prop->mData;
						std::string str(prop->mData + 4, strLength);

						if ((std::string)(prop->mKey.data) == "$raw.DiffuseColor|file")
						{
							Ref<Texture> texture = LoadMaterialTexture(str);
							if (texture)
							{
							//	material->SetAlbedoMap(texture);
							//	material->UsingAlbedoMap(true);
							}
							albedoMap = LoadMaterialTexture(str);
						}
						else if ((std::string)(prop->mKey.data) == "$raw.ShininessExponent|file")
						{
							Ref<Texture> texture = LoadMaterialTexture(str);
							if (texture)
							{
							//	material->SetRoughnessMap(texture);
							//	material->UsingRoughnessMap(true);
							}
							roughnessMap = LoadMaterialTexture(str);
						}
						else if ((std::string)(prop->mKey.data) == "$raw.NormalMap|file")
						{
							Ref<Texture> texture = LoadMaterialTexture(str);
							if (texture)
							{
							//	material->SetNormalMap(texture);
							//	material->UsingNormalMap(true);
							}
							normalMap = LoadMaterialTexture(str);
						}
						else if ((std::string)(prop->mKey.data) == "$raw.ReflectionFactor|file")
						{
							Ref<Texture> texture = LoadMaterialTexture(str);
							if (texture)
							{
							//	material->SetMetalnessMap(texture);
							//	material->UsingMetalnessMap(true);
							}
							metalnessMap = LoadMaterialTexture(str);
						}
					}
				}

				glm::vec3 albedo = glm::vec3(0.8f);
				float alpha = 1.0f;
				float roughness = 0.5f;
				float metalness = 0.0f;

				if (!albedoMap)
				{
					aiColor4D aiColor;
					if (aiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, aiColor) == AI_SUCCESS)
					{
						albedo = glm::vec3(aiColor.r, aiColor.g, aiColor.b);
						alpha = aiColor.a;
					}
				}

				if (!roughnessMap)
				{
					float shininess;

					if (aiMaterial->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS)
					{
						roughness = 1 - sqrt(shininess / 100);
					} 
				}
				if (!metalnessMap)
				{
					float aiMetalness;
					if (aiMaterial->Get(AI_MATKEY_REFLECTIVITY, aiMetalness) == AI_SUCCESS)
					{
						metalness = aiMetalness;
					}
				}

				Ref<PBRMaterial> material;
				if (alpha < 1.0f && false)
				{
					material = CreateRef<PBRMaterial>(SceneRenderer::GetGlassShader(), 100);
					material->Set("u_Alpha", alpha);
				}
				else
				{
					material = CreateRef<PBRMaterial>(SceneRenderer::GetPBRShader());
				}

				if (albedoMap)
				{

					albedo = { 1.0f, 1.0f, 1.0f };
					material->SetAlbedoMap(albedoMap);
				}
				if (normalMap)
					material->SetNormalMap(normalMap);
				if (roughnessMap)
					material->SetRoughnessMap(roughnessMap);
				if (metalnessMap)
					material->SetMetalnessMap(metalnessMap);
				material->Set("AlbedoValue", albedo);
				material->Set("RoughnessValue", roughness);
				material->Set("MetalnessValue", metalness);
				material->UsingAlbedoMap((bool)albedoMap);
				material->UsingNormalMap((bool)normalMap);
				material->UsingRoughnessMap((bool)roughnessMap);
				material->UsingMetalnessMap((bool)metalnessMap);

				m_Materials[i] = material;
			}
		}

		uint baseVertex = m_BaseVertexPointer;
		m_BaseVertexPointer += mesh->mNumVertices;
		uint baseIndex = m_BaseIndexPointer;
		uint indexCount = mesh->mNumFaces * 3;
		m_BaseIndexPointer += indexCount;
		
		submesh.vertexOffset = baseVertex;
		submesh.indexOffset = baseIndex;
		submesh.indexCount = indexCount;
		submesh.boundingBox = boundingBox;
		submesh.materialIndex = mesh->mMaterialIndex;

		return submesh;
	}

	Ref<Texture> Mesh::LoadMaterialTexture(const std::string& str)
	{
		//	NX_CORE_DEBUG("Metalness Texture: {0}", str);

		std::filesystem::path meshPath = m_Path;
		auto parentPath = meshPath.parent_path();
		parentPath /= str;
		std::string texturePath = parentPath.string();

		if (FILE* file = fopen(texturePath.c_str(), "r"))
			fclose(file);
		else
			return nullptr;

		Ref<Texture> texture = m_LoadedTextures[texturePath];
		if (texture == nullptr)
		{
			Ref<Texture> loadedTexture = CreateRef<Texture>(texturePath);
			m_LoadedTextures[texturePath] = loadedTexture;
			return loadedTexture;
		}
		else
		{
			return texture;
		}
	}

}