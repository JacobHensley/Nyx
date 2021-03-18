#include "NXpch.h"
#include "Renderer.h"
#include "Nyx/Asset/AssetManager.h"
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

#include "SceneRenderer.h"

namespace Nyx {

	struct RendererData
	{
		byte* m_UniformBuffer = nullptr;
		uint m_UniformBufferSize = 0;

		Ref<Texture> m_BRDFLutTexture;
		Ref<Shader> m_OutlineShader;

		Ref<Camera> m_ActiveCamera;
		Scene* ActiveScene = nullptr; // TEMP

		Ref<VertexArray> FullscreenQuadVA;
		Ref<IndexBuffer> FullscreenQuadIB;

		struct MaterialRef
		{
			Ref<Nyx::Material> Material;

			MaterialRef(const Ref<Nyx::Material>& material)
				: Material(material)
			{
			}

			bool operator<(const MaterialRef& other) const
			{
				return *Material.get() < *other.Material.get();
			}

			Nyx::Material& operator->()
			{
				return *Material.get();
			}
		};

		struct SubmeshDrawCommand
		{
			Nyx::SubMesh SubMesh;
			Ref<VertexArray> VA;
			Ref<IndexBuffer> IB;
			glm::mat4 Transform;
		};

		std::map<MaterialRef, std::vector<SubmeshDrawCommand>> DrawList;

		std::unordered_map<RendererID, std::function<void(const Ref<ShaderUniform>&, SubMesh&, Scene*, glm::mat4 transform)>> m_RendererUniformFuncs;
		std::unordered_map<RendererID, std::function<void(const Ref<ShaderResource>&, Scene*)>> m_RendererResourceFuncs;
	};

	static RendererData s_Data;

	void Renderer::Init()
	{
		InitRenderFunctions();
		InitRenderResourceFunFunctions();

		s_Data.m_BRDFLutTexture = CreateRef<Texture>("assets/textures/Brdf_Lut.png");
		s_Data.m_OutlineShader = CreateRef<Shader>("assets/shaders/outlineShader.shader");

		// Fullscreen Quad
		{
			s_Data.FullscreenQuadVA = CreateRef<VertexArray>();

			struct QuadVertex
			{
				glm::vec2 Position;
				glm::vec2 TexCoord;
			};

			std::array<QuadVertex, 4> vertices;

			vertices[0].Position = { -1.0f, -1.0f };
			vertices[0].TexCoord = { 0.0f, 0.0f };

			vertices[1].Position = { 1.0f, -1.0f };
			vertices[1].TexCoord = { 1.0f, 0.0f };

			vertices[2].Position = { 1.0f,  1.0f };
			vertices[2].TexCoord = { 1.0f, 1.0f };

			vertices[3].Position = { -1.0f,  1.0f };
			vertices[3].TexCoord = { 0.0f, 1.0f };

			Ref<VertexBuffer> fullscreenQuadVB = CreateRef<VertexBuffer>(vertices.data(), vertices.size() * sizeof(QuadVertex));
			s_Data.FullscreenQuadVA->PushVertexBuffer(fullscreenQuadVB);

			uint32_t fullscreenQuadIndices[] = { 0, 1, 2, 2, 3, 0 };
			s_Data.FullscreenQuadIB = CreateRef<IndexBuffer>(fullscreenQuadIndices, 6);
		}

	}

	void Renderer::Begin(Ref<Camera> camera, Scene* scene)
	{
		s_Data.m_ActiveCamera = camera;
		s_Data.ActiveScene = scene;

		struct UB
		{
			glm::mat4 ViewProjection;
			glm::mat4 InverseVP;
		} ub;
		ub.ViewProjection = camera->GetViewMatrix();
		ub.InverseVP = glm::inverse(ub.ViewProjection);

		Shader::UploadUniformBuffer(0, (byte*)&ub, sizeof(UB));
	}

	// Material sorting... but on a mesh level -> per SCENE
	void Renderer::SubmitMesh(Scene* scene, Ref<Mesh> mesh, const glm::mat4& transform)
	{
		auto& subMeshes = mesh->GetSubMeshs();
		const auto& materials = mesh->GetMaterials();

		for (SubMesh& subMesh : subMeshes)
		{
			Ref<Material> material = materials[subMesh.materialIndex];
			// submit all submeshes
			s_Data.DrawList[material].push_back({ subMesh, mesh->GetVertexArray(), mesh->GetIndexBuffer(), transform * subMesh.transform });
		}

#if 0
		std::vector<SubMesh>& subMeshes = mesh->GetSubMeshs();

		mesh->GetVertexArray()->Bind();
		mesh->GetIndexBuffer()->Bind();

		const auto & meshMaterials = mesh->GetMaterials();
		std::unordered_map<Ref<Shader>, std::unordered_map<uint32_t, std::vector<SubMesh*>>> materials;
		for (SubMesh& subMesh : subMeshes)
		{
			const auto & submeshMaterial = meshMaterials[subMesh.materialIndex];
			materials[submeshMaterial->GetShader()][subMesh.materialIndex].emplace_back(&subMesh);
		}
		
		for (auto& [shader, materialMap] : materials)
		{
			shader->Bind();
			
			std::vector<UniformBuffer*> uniformBuffers = shader->GetUniformBuffers(UniformSystemType::RENDERER);
			
			std::vector<Ref<ShaderResource>> resources = shader->GetResources(UniformSystemType::RENDERER);
			for (Ref<ShaderResource> resource : resources)
			{
				RendererID rendererID = resource->GetRendererID();
				s_Data.m_RendererResourceFuncs[rendererID](resource, scene);
			}
			
			for (auto& [materialIndex, submeshList] : materialMap)
			{
				auto & material = meshMaterials[materialIndex];
				material->BindTextures();
				material->UploadUniformBuffers();
				
				for (SubMesh* subMesh : submeshList)
				{
					for (UniformBuffer* uniformBuffer : uniformBuffers)
					{
					//	if (!s_Data.m_UniformBuffer || s_Data.m_UniformBufferSize < uniformBuffer->size)
						{
						//	delete[] s_Data.m_UniformBuffer;
							s_Data.m_UniformBuffer = new byte[uniformBuffer->size];
							s_Data.m_UniformBufferSize = uniformBuffer->size;
						}
						
						std::vector<Ref<ShaderUniform>> uniforms = uniformBuffer->uniforms;
						
						for (Ref<ShaderUniform> uniform : uniforms)
						{
							s_Data.m_RendererUniformFuncs[uniform->GetRendererID()](uniform, *subMesh, scene, transform);
						}
						
						shader->UploadUniformBuffer(uniformBuffer->index, s_Data.m_UniformBuffer, s_Data.m_UniformBufferSize);
					//	delete s_Data.m_UniformBuffer;
					}

					glDrawElementsBaseVertex(GL_TRIANGLES, subMesh->indexCount, GL_UNSIGNED_INT, (void*)(subMesh->indexOffset * sizeof(uint)), subMesh->vertexOffset);
				}
			}
		}
#endif
	}

	void Renderer::SubmitMesh(Scene* scene, Ref<Mesh> mesh, const glm::mat4& transform, Ref<Material> material)
	{
		auto& subMeshes = mesh->GetSubMeshs();
		const auto& materials = mesh->GetMaterials();

		for (SubMesh& subMesh : subMeshes)
		{
			Ref<Material> submeshMaterial = material ? material : materials[subMesh.materialIndex];
			// submit all submeshes
			s_Data.DrawList[material].push_back({ subMesh, mesh->GetVertexArray(), mesh->GetIndexBuffer(), transform * subMesh.transform });
		}

#if 0

		mesh->GetVertexArray()->Bind();
		mesh->GetIndexBuffer()->Bind();

		for (SubMesh subMesh : subMeshes)
		{
			material->Bind();
			Ref<Shader> shader = material->GetShader();

			std::vector<UniformBuffer*> uniformBuffers = shader->GetUniformBuffers(UniformSystemType::RENDERER);

			for (UniformBuffer* uniformBuffer : uniformBuffers)
			{
			//	if (!s_Data.m_UniformBuffer || s_Data.m_UniformBufferSize < uniformBuffer->size)
				{
				//	delete[] s_Data.m_UniformBuffer;
					s_Data.m_UniformBuffer = new byte[uniformBuffer->size];
					s_Data.m_UniformBufferSize = uniformBuffer->size;
				}

				std::vector<Ref<ShaderUniform>> uniforms = uniformBuffer->uniforms;

				for (Ref<ShaderUniform> uniform : uniforms)
				{
					s_Data.m_RendererUniformFuncs[uniform->GetRendererID()](uniform, subMesh, scene, transform);
				}

				shader->UploadUniformBuffer(uniformBuffer->index, s_Data.m_UniformBuffer, s_Data.m_UniformBufferSize);
			//	delete s_Data.m_UniformBuffer;
			}

			std::vector<Ref<ShaderResource>> resources = shader->GetResources(UniformSystemType::RENDERER);
			for (Ref<ShaderResource> resource : resources)
			{
				RendererID rendererID = resource->GetRendererID();
				s_Data.m_RendererResourceFuncs[rendererID](resource, scene);
			}

			if (!material->GetDepthTesting())
				glDisable(GL_DEPTH_TEST);

			glDrawElementsBaseVertex(GL_TRIANGLES, subMesh.indexCount, GL_UNSIGNED_INT, (void*)(subMesh.indexOffset * sizeof(uint)), subMesh.vertexOffset);

			if (!material->GetDepthTesting())
				glEnable(GL_DEPTH_TEST);
		}
#endif
	}

	void Renderer::SubmitFullscreenQuad(Ref<Material> material)
	{
		material->Bind();

		s_Data.FullscreenQuadVA->Bind();
		s_Data.FullscreenQuadIB->Bind();
		//glDepthMask(GL_FALSE);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
		//glDepthMask(GL_TRUE);
	}

	void Renderer::End()
	{
		Flush();
	}

	void Renderer::Flush()
	{
		for (auto& [materialRef, meshList] : s_Data.DrawList)
		{
			auto cameraPosition = s_Data.m_ActiveCamera->GetPosition();
			Ref<Material> material = materialRef.Material;
			std::sort(meshList.begin(), meshList.end(), [material, cameraPosition](auto& a, auto& b)
			{
				float camMeshA = glm::length(cameraPosition - glm::vec3(a.Transform[3]));
				float camMeshB = glm::length(cameraPosition - glm::vec3(b.Transform[3]));
				if (!material->IsOpaque())
					return camMeshA > camMeshB;

				return camMeshA < camMeshB;
			});

			auto shader = material->GetShader();
			shader->Bind();

			std::vector<UniformBuffer*> uniformBuffers = shader->GetUniformBuffers(UniformSystemType::RENDERER);

			material->Bind();

			for (auto& submeshDC : meshList)
			{
				SubMesh& submesh = submeshDC.SubMesh;

				submeshDC.VA->Bind();
				submeshDC.IB->Bind();

				glm::mat4& transform = submeshDC.Transform;

#if 0
				for (UniformBuffer* uniformBuffer : uniformBuffers)
				{
					//	if (!s_Data.m_UniformBuffer || s_Data.m_UniformBufferSize < uniformBuffer->size)
					{
						//	delete[] s_Data.m_UniformBuffer;
						s_Data.m_UniformBuffer = new byte[uniformBuffer->size];
						s_Data.m_UniformBufferSize = uniformBuffer->size;
					}

					std::vector<Ref<ShaderUniform>> uniforms = uniformBuffer->uniforms;

					for (Ref<ShaderUniform> uniform : uniforms)
					{
						s_Data.m_RendererUniformFuncs[uniform->GetRendererID()](uniform, submesh, s_Data.ActiveScene, transform);
					}

					shader->UploadUniformBuffer(uniformBuffer->index, s_Data.m_UniformBuffer, s_Data.m_UniformBufferSize);
					//	delete s_Data.m_UniformBuffer;
				}
#endif

				std::vector<Ref<ShaderResource>> resources = shader->GetResources(UniformSystemType::RENDERER);
				for (Ref<ShaderResource> resource : resources)
				{
					RendererID rendererID = resource->GetRendererID();
					s_Data.m_RendererResourceFuncs[rendererID](resource, s_Data.ActiveScene);
				}

				glDrawElementsBaseVertex(GL_TRIANGLES, submesh.indexCount, GL_UNSIGNED_INT, (void*)(submesh.indexOffset * sizeof(uint)), submesh.vertexOffset);

			}

		}

		s_Data.DrawList.clear();
	}

	void Renderer::InitRenderFunctions()
	{
		s_Data.m_RendererUniformFuncs[RendererID::MODEL_MATRIX] = [&](const Ref<ShaderUniform>& uniform, SubMesh& mesh, Scene* scene, glm::mat4 transform)
		{
			auto finalTransform = transform * mesh.transform;
 			memcpy(s_Data.m_UniformBuffer + uniform->GetOffset(), &finalTransform, uniform->GetSize());
		};
		s_Data.m_RendererUniformFuncs[RendererID::VIEW_MATRIX] = [&](const Ref<ShaderUniform>& uniform, SubMesh& mesh, Scene* scene, glm::mat4 transform)
		{
			auto view = s_Data.m_ActiveCamera->GetViewMatrix();
			memcpy(s_Data.m_UniformBuffer + uniform->GetOffset(), glm::value_ptr(view), uniform->GetSize());
		};
		s_Data.m_RendererUniformFuncs[RendererID::PROJ_MATRIX] = [&](const Ref<ShaderUniform>& uniform, SubMesh& mesh, Scene* scene, glm::mat4 transform)
		{
			auto proj = s_Data.m_ActiveCamera->GetProjectionMatrix();
			memcpy(s_Data.m_UniformBuffer + uniform->GetOffset(), glm::value_ptr(proj), uniform->GetSize());
		};
		s_Data.m_RendererUniformFuncs[RendererID::INVERSE_VP] = [&](const Ref<ShaderUniform>& uniform, SubMesh& mesh, Scene* scene, glm::mat4 transform)
		{
			auto ivp = glm::inverse(s_Data.m_ActiveCamera->GetViewMatrix() * s_Data.m_ActiveCamera->GetProjectionMatrix());
			memcpy(s_Data.m_UniformBuffer + uniform->GetOffset(), glm::value_ptr(ivp), uniform->GetSize());
		};
		s_Data.m_RendererUniformFuncs[RendererID::MVP] = [&](const Ref<ShaderUniform>& uniform, SubMesh& mesh, Scene* scene, glm::mat4 transform)
		{
			auto mvp = s_Data.m_ActiveCamera->GetProjectionMatrix() * s_Data.m_ActiveCamera->GetViewMatrix() * (transform * mesh.transform);
			memcpy(s_Data.m_UniformBuffer + uniform->GetOffset(), &mvp, uniform->GetSize());
		};
		s_Data.m_RendererUniformFuncs[RendererID::CAMERA_POSITION] = [&](const Ref<ShaderUniform>& uniform, SubMesh& mesh, Scene* scene, glm::mat4 transform)
		{
			auto pos = s_Data.m_ActiveCamera->GetPosition();
			memcpy(s_Data.m_UniformBuffer + uniform->GetOffset(), glm::value_ptr(pos), uniform->GetSize());
		};
		s_Data.m_RendererUniformFuncs[RendererID::DIRECTIONAL_LIGHT] = [&](const Ref<ShaderUniform>& uniform, SubMesh& mesh, Scene* scene, glm::mat4 transform)
		{
			auto light = scene->GetLightEnvironment()->GetDirectionalLight();
			DirectionalLight l = *light;
			uint s = sizeof(DirectionalLight);
			memcpy(s_Data.m_UniformBuffer + uniform->GetOffset(), &l.radiance, sizeof(glm::vec3));
			memcpy(s_Data.m_UniformBuffer + uniform->GetOffset() + sizeof(glm::vec3) + 4, &l.direction, sizeof(glm::vec3));
			memcpy(s_Data.m_UniformBuffer + uniform->GetOffset() + sizeof(glm::vec3) + 4 + sizeof(glm::vec3) + 4, &l.active, sizeof(float));
		};
		s_Data.m_RendererUniformFuncs[RendererID::POINT_LIGHT] = [&](const Ref<ShaderUniform>& uniform, SubMesh& mesh, Scene* scene, glm::mat4 transform)
		{
			auto light = scene->GetLightEnvironment()->GetPointLight();
			glm::vec4 radiance = glm::vec4(light.radiance, light.intensity);
			memcpy(s_Data.m_UniformBuffer + uniform->GetOffset(), &light.position, sizeof(glm::vec3));
			memcpy(s_Data.m_UniformBuffer + uniform->GetOffset() + sizeof(glm::vec3) + 4, &radiance, sizeof(glm::vec4));
		};
	}
	void Renderer::InitRenderResourceFunFunctions()
	{
		s_Data.m_RendererResourceFuncs[RendererID::IRRADIANCE_TEXTURE] = [&](const Ref<ShaderResource>& uniform, Scene* scene)
		{
			AssetManager::GetByUUID<TextureCube>(scene->GetEnvironmentMap()->irradianceMap.GetUUID())->Bind(uniform->GetSampler());
		};
		s_Data.m_RendererResourceFuncs[RendererID::RADIANCE_TEXTURE] = [&](const Ref<ShaderResource>& uniform, Scene* scene)
		{
			AssetManager::GetByUUID<TextureCube>(scene->GetEnvironmentMap()->radianceMap.GetUUID())->Bind(uniform->GetSampler());
		};
		s_Data.m_RendererResourceFuncs[RendererID::BRDF_LUT] = [&](const Ref<ShaderResource>& uniform, Scene* scene)
		{
			s_Data.m_BRDFLutTexture->Bind(uniform->GetSampler());
		};
	}

}