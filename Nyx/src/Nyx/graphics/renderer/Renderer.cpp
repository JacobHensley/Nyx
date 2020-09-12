#include "NXpch.h"
#include "Renderer.h"
#include "glm/gtc/type_ptr.hpp"
#include <glad/glad.h>
#include "Nyx/AssetManager.h"

namespace Nyx {

	struct RendererData
	{
		byte* m_UniformBuffer = nullptr;
		uint m_UniformBufferSize = 0;

		Ref<Texture> m_BRDFLutTexture;
		Ref<Shader> m_OutlineShader;

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

	}

	// Material sorting... but on a mesh level -> per SCENE
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

					//This system is terrible completly rewrite the whole thing just wanted to see if i could get it work late at night at surgery
					//1. structure in way as to not have to rebind shader this is very bad
					//2. structure that makes choosing to render an outline or not easier
					//3. in the drawWireframe method remove all the extra code for uploading uniforms dynamicly
					//4. Find out correct order to render this in becuse it needs to render below the object but over everything else 
					//   - this is going to be the hardest i think and might need to be done first as to know when to bind shaders
					//5. Add point rendering to it so that it smooths out the corners
					//6. Add settings for color and size
					//7. also maybe grow some balls and dont be afraid of the jump flood algo
				//	DrawWireframe(scene, mesh, transform);
				//	shader->Bind();
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
	}

	void Renderer::DrawWireframe(Scene* scene, Ref<Mesh> mesh, glm::mat4 transform)
	{
		std::vector<SubMesh>& subMeshes = mesh->GetSubMeshs();

		mesh->GetVertexArray()->Bind();
		mesh->GetIndexBuffer()->Bind();

		transform = glm::translate(transform, glm::vec3(1.0f, 1.0f, 1.0f));

		for (SubMesh subMesh : subMeshes)
		{
			Ref<Shader> shader = s_Data.m_OutlineShader;
			shader->Bind();
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

			glDisable(GL_DEPTH_TEST);
			
			// Push the GL attribute bits so that we don't wreck any settings
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			// Enable polygon offsets, and offset filled polygons forward by 2.5
			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(-2.5f, -2.5f);
			// Set the render mode to be line rendering with a thick line width
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glLineWidth(10.0f);
			// Set the colour to be white
			glColor3f(1.0f, 1.0f, 1.0f);
			// Render the object
			glDrawElementsBaseVertex(GL_TRIANGLES, subMesh.indexCount, GL_UNSIGNED_INT, (void*)(subMesh.indexOffset * sizeof(uint)), subMesh.vertexOffset);
			// Set the polygon mode to be filled triangles 

			// Pop the state changes off the attribute stack
			// to set things back how they were
			glPopAttrib();
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glEnable(GL_LIGHTING);
			// Set the colour to the background
			glColor3f(0.0f, 0.0f, 0.0f);
			glEnable(GL_DEPTH_TEST);
		}
	}

	void Renderer::InitRenderFunctions()
	{
		s_Data.m_RendererUniformFuncs[RendererID::MODEL_MATRIX] = [&](const Ref<ShaderUniform>& uniform, SubMesh& mesh, Scene* scene, glm::mat4 transform)
		{
			auto finalTransform = transform * mesh.transform; // 
 			memcpy(s_Data.m_UniformBuffer + uniform->GetOffset(), &finalTransform, uniform->GetSize());
		};
		s_Data.m_RendererUniformFuncs[RendererID::VIEW_MATRIX] = [&](const Ref<ShaderUniform>& uniform, SubMesh& mesh, Scene* scene, glm::mat4 transform)
		{
			auto view = scene->GetCamera()->GetViewMatrix();
			memcpy(s_Data.m_UniformBuffer + uniform->GetOffset(), glm::value_ptr(view), uniform->GetSize());
		};
		s_Data.m_RendererUniformFuncs[RendererID::PROJ_MATRIX] = [&](const Ref<ShaderUniform>& uniform, SubMesh& mesh, Scene* scene, glm::mat4 transform)
		{
			auto proj = scene->GetCamera()->GetProjectionMatrix();
			memcpy(s_Data.m_UniformBuffer + uniform->GetOffset(), glm::value_ptr(proj), uniform->GetSize());
		};
		s_Data.m_RendererUniformFuncs[RendererID::INVERSE_VP] = [&](const Ref<ShaderUniform>& uniform, SubMesh& mesh, Scene* scene, glm::mat4 transform)
		{
			auto ivp = glm::inverse(scene->GetCamera()->GetViewMatrix() * scene->GetCamera()->GetProjectionMatrix());
			memcpy(s_Data.m_UniformBuffer + uniform->GetOffset(), glm::value_ptr(ivp), uniform->GetSize());
		};
		s_Data.m_RendererUniformFuncs[RendererID::MVP] = [&](const Ref<ShaderUniform>& uniform, SubMesh& mesh, Scene* scene, glm::mat4 transform)
		{
			auto mvp = scene->GetCamera()->GetProjectionMatrix() * scene->GetCamera()->GetViewMatrix() * (transform * mesh.transform);
			memcpy(s_Data.m_UniformBuffer + uniform->GetOffset(), &mvp, uniform->GetSize());
		};
		s_Data.m_RendererUniformFuncs[RendererID::CAMERA_POSITION] = [&](const Ref<ShaderUniform>& uniform, SubMesh& mesh, Scene* scene, glm::mat4 transform)
		{
			auto pos = scene->GetCamera()->GetPosition();
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