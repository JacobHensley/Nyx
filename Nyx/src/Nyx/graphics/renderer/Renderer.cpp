#include "NXpch.h"
#include "Renderer.h"
#include "glm/gtc/type_ptr.hpp"
#include <glad/glad.h>

namespace Nyx {

	struct RendererData
	{
		byte* m_UniformBuffer;
		uint m_UniformBufferSize;

		Ref<Texture> m_BRDFLutTexture;

		std::unordered_map<RendererID, std::function<void(const Ref<ShaderUniform>&, SubMesh&, Scene*)>> m_RendererUniformFuncs;
		std::unordered_map<RendererID, std::function<void(const Ref<ShaderResource>&, Scene*)>> m_RendererResourceFuncs;
	};

	static struct RendererData s_Data;

	void Renderer::Init()
	{
		InitRenderFunctions();
		InitRenderResourceFunFunctions();

		s_Data.m_BRDFLutTexture = CreateRef<Texture>("assets/textures/Brdf_Lut.png");

	}

	void Renderer::SubmitMesh(Scene* scene, Ref<Mesh> mesh, glm::mat4 transform)
	{
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
						s_Data.m_UniformBufferSize = uniformBuffer->size;
						s_Data.m_UniformBuffer = new byte[uniformBuffer->size];
						
						std::vector<Ref<ShaderUniform>> uniforms = uniformBuffer->uniforms;
						
						for (Ref<ShaderUniform> uniform : uniforms)
						{
							s_Data.m_RendererUniformFuncs[uniform->GetRendererID()](uniform, *subMesh, scene);
						}
						
						shader->UploadUniformBuffer(uniformBuffer->index, s_Data.m_UniformBuffer, s_Data.m_UniformBufferSize);
					//	delete s_Data.m_UniformBuffer;
					}

					glDrawElementsBaseVertex(GL_TRIANGLES, subMesh->indexCount, GL_UNSIGNED_INT, (void*)(subMesh->indexOffset * sizeof(uint)), subMesh->vertexOffset);
				}
			}
		}
	}

	void Renderer::SubmitMesh(Scene* scene, Ref<Mesh> mesh, glm::mat4 transform, Ref<Material> material)
	{
		std::vector<SubMesh>& subMeshes = mesh->GetSubMeshs();

		mesh->GetVertexArray()->Bind();
		mesh->GetIndexBuffer()->Bind();

		for (SubMesh subMesh : subMeshes)
		{
			material->Bind();
			Ref<Shader> shader = material->GetShader();

			std::vector<UniformBuffer*> uniformBuffers = shader->GetUniformBuffers(UniformSystemType::RENDERER);

			for (UniformBuffer* uniformBuffer : uniformBuffers)
			{
				s_Data.m_UniformBufferSize = uniformBuffer->size;
				s_Data.m_UniformBuffer = new byte[uniformBuffer->size];

				std::vector<Ref<ShaderUniform>> uniforms = uniformBuffer->uniforms;

				for (Ref<ShaderUniform> uniform : uniforms)
				{
					s_Data.m_RendererUniformFuncs[uniform->GetRendererID()](uniform, subMesh, scene);
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
	}

	void Renderer::InitRenderFunctions()
	{
		s_Data.m_RendererUniformFuncs[RendererID::MODEL_MATRIX] = [&](const Ref<ShaderUniform>& uniform, SubMesh& mesh, Scene* scene)
		{
			auto transform = mesh.transform;
 			memcpy(s_Data.m_UniformBuffer + uniform->GetOffset(), &transform, uniform->GetSize());
		};
		s_Data.m_RendererUniformFuncs[RendererID::VIEW_MATRIX] = [&](const Ref<ShaderUniform>& uniform, SubMesh& mesh, Scene* scene)
		{
			auto view = scene->GetCamera()->GetViewMatrix();
			memcpy(s_Data.m_UniformBuffer + uniform->GetOffset(), glm::value_ptr(view), uniform->GetSize());
		};
		s_Data.m_RendererUniformFuncs[RendererID::PROJ_MATRIX] = [&](const Ref<ShaderUniform>& uniform, SubMesh& mesh, Scene* scene)
		{
			auto proj = scene->GetCamera()->GetProjectionMatrix();
			memcpy(s_Data.m_UniformBuffer + uniform->GetOffset(), glm::value_ptr(proj), uniform->GetSize());
		};
		s_Data.m_RendererUniformFuncs[RendererID::INVERSE_VP] = [&](const Ref<ShaderUniform>& uniform, SubMesh& mesh, Scene* scene)
		{
			auto ivp = glm::inverse(scene->GetCamera()->GetViewMatrix() * scene->GetCamera()->GetProjectionMatrix());
			memcpy(s_Data.m_UniformBuffer + uniform->GetOffset(), glm::value_ptr(ivp), uniform->GetSize());
		};
		s_Data.m_RendererUniformFuncs[RendererID::MVP] = [&](const Ref<ShaderUniform>& uniform, SubMesh& mesh, Scene* scene)
		{
			auto mvp = scene->GetCamera()->GetProjectionMatrix() * scene->GetCamera()->GetViewMatrix() * mesh.transform;
			memcpy(s_Data.m_UniformBuffer + uniform->GetOffset(), &mvp, uniform->GetSize());
		};
		s_Data.m_RendererUniformFuncs[RendererID::CAMERA_POSITION] = [&](const Ref<ShaderUniform>& uniform, SubMesh& mesh, Scene* scene)
		{
			auto pos = scene->GetCamera()->GetPosition();
			memcpy(s_Data.m_UniformBuffer + uniform->GetOffset(), glm::value_ptr(pos), uniform->GetSize());
		};
		s_Data.m_RendererUniformFuncs[RendererID::DIRECTIONAL_LIGHT] = [&](const Ref<ShaderUniform>& uniform, SubMesh& mesh, Scene* scene)
		{
			auto light = scene->GetLightEnvironment()->GetDirectionalLight();
			DirectionalLight l = *light;
			uint s = sizeof(DirectionalLight);
			memcpy(s_Data.m_UniformBuffer + uniform->GetOffset(), &l.radiance, sizeof(glm::vec3));
			memcpy(s_Data.m_UniformBuffer + uniform->GetOffset() + sizeof(glm::vec3) + 4, &l.direction, sizeof(glm::vec3));
			memcpy(s_Data.m_UniformBuffer + uniform->GetOffset() + sizeof(glm::vec3) + 4 + sizeof(glm::vec3) + 4, &l.active, sizeof(float));
		};
	}
	void Renderer::InitRenderResourceFunFunctions()
	{
		s_Data.m_RendererResourceFuncs[RendererID::IRRADIANCE_TEXTURE] = [&](const Ref<ShaderResource>& uniform, Scene* scene)
		{
			scene->GetEnvironmentMap()->irradianceMap->Bind(uniform->GetSampler());
		};
		s_Data.m_RendererResourceFuncs[RendererID::RADIANCE_TEXTURE] = [&](const Ref<ShaderResource>& uniform, Scene* scene)
		{
			scene->GetEnvironmentMap()->radianceMap->Bind(uniform->GetSampler());
		};
		s_Data.m_RendererResourceFuncs[RendererID::BRDF_LUT] = [&](const Ref<ShaderResource>& uniform, Scene* scene)
		{
			s_Data.m_BRDFLutTexture->Bind(uniform->GetSampler());
		};
	}

}