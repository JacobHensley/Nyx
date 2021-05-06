#include "NXpch.h"
#include "Mesh.h"
#include "Nyx/graphics/SceneRenderer.h"
#include "Nyx/Asset/AssetManager.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

namespace Nyx {
	
	namespace Utils {

		static inline glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4 from)
		{
			glm::mat4 to;
			to[0][0] = (float)from.a1; to[0][1] = (float)from.b1;  to[0][2] = (float)from.c1; to[0][3] = (float)from.d1;
			to[1][0] = (float)from.a2; to[1][1] = (float)from.b2;  to[1][2] = (float)from.c2; to[1][3] = (float)from.d2;
			to[2][0] = (float)from.a3; to[2][1] = (float)from.b3;  to[2][2] = (float)from.c3; to[2][3] = (float)from.d3;
			to[3][0] = (float)from.a4; to[3][1] = (float)from.b4;  to[3][2] = (float)from.c4; to[3][3] = (float)from.d4;
			return to;
		}

	}

	Mesh::Mesh(const std::string& path)
		: m_Path(path)
	{
		NX_CORE_INFO("Loading mesh at path {0}", m_Path);
		bool loaded = Load(m_Path);
		NX_CORE_ASSERT(loaded, "Failed to load model");
		NX_CORE_INFO("Created Mesh at Path: {0}", m_Path);
	}

	bool Mesh::Load(const std::string& path)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

		if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			NX_CORE_ERROR("Error loading mesh {0}: {1}", path, importer.GetErrorString());
			return false;
		}

		LoadData(scene);
		LoadMaterials(scene);
		CalculateNodeTransforms(scene->mRootNode, scene, scene->mRootNode->mTransformation);

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

	void Mesh::LoadData(const aiScene* scene)
	{
		for (uint i = 0; i < scene->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[i];
			SubMesh& subMesh = m_SubMeshes.emplace_back();

			for (uint i = 0; i < mesh->mNumVertices; i++)
			{
				Vertex& vertex = m_Vertices.emplace_back();

				// Positions
				vertex.Position.x = mesh->mVertices[i].x;
				vertex.Position.y = mesh->mVertices[i].y;
				vertex.Position.z = mesh->mVertices[i].z;

				// Normals
				vertex.Normal.x = mesh->mNormals[i].x;
				vertex.Normal.y = mesh->mNormals[i].y;
				vertex.Normal.z = mesh->mNormals[i].z;

				// Binormals
				vertex.Binormal.x = mesh->mBitangents[i].x;
				vertex.Binormal.y = mesh->mBitangents[i].y;
				vertex.Binormal.z = mesh->mBitangents[i].z;

				// Tangents
				vertex.Tangent.x = mesh->mTangents[i].x;
				vertex.Tangent.y = mesh->mTangents[i].y;
				vertex.Tangent.z = mesh->mTangents[i].z;

				// Texture Coordinates
				if (mesh->HasTextureCoords(0))
				{
					vertex.TextureCoords.x = mesh->mTextureCoords[0][i].x;
					vertex.TextureCoords.y = mesh->mTextureCoords[0][i].y;
				}
				else
				{
					vertex.TextureCoords = glm::vec2(0.0f, 0.0f);
				}

				// Bounding Box
				subMesh.BoundingBox.Min.x = glm::min(mesh->mVertices[i].x, subMesh.BoundingBox.Min.x);
				subMesh.BoundingBox.Min.y = glm::min(mesh->mVertices[i].y, subMesh.BoundingBox.Min.y);
				subMesh.BoundingBox.Min.z = glm::min(mesh->mVertices[i].z, subMesh.BoundingBox.Min.z);

				subMesh.BoundingBox.Max.x = glm::max(mesh->mVertices[i].x, subMesh.BoundingBox.Max.x);
				subMesh.BoundingBox.Max.y = glm::max(mesh->mVertices[i].y, subMesh.BoundingBox.Max.y);
				subMesh.BoundingBox.Max.z = glm::max(mesh->mVertices[i].z, subMesh.BoundingBox.Max.z);
			}

			// Triangles and Indices
			for (uint i = 0; i < mesh->mNumFaces; i++)
			{
				aiFace face = mesh->mFaces[i];
				Triangle& triangle = m_Triangles.emplace_back();

				for (uint j = 0; j < face.mNumIndices; j++)
				{
					triangle.Points[j].x = mesh->mVertices[face.mIndices[j]].x;
					triangle.Points[j].y = mesh->mVertices[face.mIndices[j]].y;
					triangle.Points[j].z = mesh->mVertices[face.mIndices[j]].z;

					m_Indices.push_back(face.mIndices[j]);
				}
			}
			
			// Set subMesh data
			uint32_t baseVertex = m_BaseVertexPointer;
			m_BaseVertexPointer += mesh->mNumVertices;

			uint32_t baseIndex = m_BaseIndexPointer;
			uint32_t indexCount = mesh->mNumFaces * 3;
			m_BaseIndexPointer += indexCount;

			uint32_t baseTriangle = m_BaseTrianglePointer;
			uint32_t triangleCount = mesh->mNumFaces;
			m_BaseTrianglePointer += triangleCount;

			subMesh.VertexOffset = baseVertex;
			subMesh.IndexOffset = baseIndex;
			subMesh.IndexCount = indexCount;
			subMesh.TriangleOffset = baseTriangle;
			subMesh.TriangleCount = triangleCount;
			subMesh.MaterialIndex = mesh->mMaterialIndex;
		}
	}

	void Mesh::LoadMaterials(const aiScene* scene)
	{
		if (scene->HasMaterials())
		{
			m_Materials.resize(scene->mNumMaterials);
			for (uint32_t i = 0; i < scene->mNumMaterials; i++)
			{
				auto aiMaterial = scene->mMaterials[i];
				auto aiMaterialName = aiMaterial->GetName();

				// Load material textures
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
							albedoMap = LoadMaterialTexture(str);
						}
						else if ((std::string)(prop->mKey.data) == "$raw.ShininessExponent|file")
						{
							roughnessMap = LoadMaterialTexture(str);
						}
						else if ((std::string)(prop->mKey.data) == "$raw.NormalMap|file")
						{
							normalMap = LoadMaterialTexture(str);
						}
						else if ((std::string)(prop->mKey.data) == "$raw.ReflectionFactor|file")
						{
							metalnessMap = LoadMaterialTexture(str);
						}
					}
				}

				// Load material values
				glm::vec3 albedo = glm::vec3(0.8f);
				float roughness = 0.5f;
				float metalness = 0.0f;
				float alpha = 1.0f;

				aiColor4D aiColor;
				if (aiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, aiColor) == AI_SUCCESS)
				{
					albedo = glm::vec3(aiColor.r, aiColor.g, aiColor.b);
				}

				float aiShininess;
				if (aiMaterial->Get(AI_MATKEY_SHININESS, aiShininess) == AI_SUCCESS)
				{
					roughness = 1 - sqrt(aiShininess / 100);
				}

				float aiMetalness;
				if (aiMaterial->Get(AI_MATKEY_REFLECTIVITY, aiMetalness) == AI_SUCCESS)
				{
					metalness = aiMetalness;
				}

				float aiOpacity;
				if (aiMaterial->Get(AI_MATKEY_OPACITY, aiOpacity) == AI_SUCCESS)
				{
					alpha = aiOpacity;
				}

				// Create material
				Ref<Material> material;
				if (alpha < 1.0f)
				{
					material = CreateRef<Material>(SceneRenderer::GetGlassShader());
					material->SetMaterialSortKey(100);
					material->Set("u_Material.u_Alpha", alpha);
				}
				else
				{
					material = CreateRef<Material>(SceneRenderer::GetPBRShader());
				}

				Ref<Texture> defaultTexture = CreateRef<Texture>(1, 1);
				defaultTexture->SetData(new byte[4], 4);

				// Albedo
				if (albedoMap)
				{
					material->SetTexture("u_AlbedoMap", albedoMap);
					albedo = glm::vec3(1.0f);
				}
				else
				{
					material->SetTexture("u_AlbedoMap", defaultTexture);
				}

				// Roughness
				if (roughnessMap)
				{
					material->SetTexture("u_RoughnessMap", roughnessMap);
					roughness = 1.0f;
				}
				else
				{
					material->SetTexture("u_RoughnessMap", defaultTexture);
				}
				
				// Metalness
				if (metalnessMap)
				{
					material->SetTexture("u_MetalnessMap", metalnessMap);
					metalness = 1.0f;
				}
				else
				{
					material->SetTexture("u_MetalnessMap", defaultTexture);
				}

				// Normal
				if (normalMap)
				{
					material->SetTexture("u_NormalMap", normalMap);
				}
				else
				{
					material->SetTexture("u_NormalMap", defaultTexture);
				}

				material->Set("u_Material.AlbedoValue", albedo);
				material->Set("u_Material.RoughnessValue", roughness);
				material->Set("u_Material.MetalnessValue", metalness);

				material->Set("u_Material.UseNormalMap", (bool)normalMap);

				m_Materials[i] = material;
			}
		}
	}

	Ref<Texture> Mesh::LoadMaterialTexture(const std::string& path)
	{
		// Get texture path
		std::filesystem::path meshPath = m_Path;
		auto parentPath = meshPath.parent_path();
		parentPath /= path;
		std::string texturePath = parentPath.string();

		// Check for texture
		if (FILE* file = fopen(texturePath.c_str(), "r"))
			fclose(file);
		else
			return nullptr;

		AssetHandle textureHandle = AssetManager::Load<Texture>(texturePath);

		return AssetManager::Get<Texture>(textureHandle);
	}

	void Mesh::CalculateNodeTransforms(aiNode* node, const aiScene* scene, aiMatrix4x4 parentTransform)
	{
		aiMatrix4x4 transform = parentTransform * node->mTransformation;

		for (uint i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			m_SubMeshes[i].Transform = Utils::aiMatrix4x4ToGlm(transform);
		}

		for (uint i = 0; i < node->mNumChildren; i++)
		{
			CalculateNodeTransforms(node->mChildren[i], scene, transform);
		}
	}

	bool Mesh::Reload()
	{
		return false;
	}
}