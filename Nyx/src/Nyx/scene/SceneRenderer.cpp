#include "NXpch.h"
#include "SceneRenderer.h"
#include "Nyx/graphics/renderer/Renderer.h"
#include "Nyx/graphics/MeshFactory.h"

#include "glm/gtc/type_ptr.hpp"

#include <glad/glad.h>

namespace Nyx {

	struct SceneRendererData
	{
		Scene* m_ActiveScene;

		std::vector<RenderCommand> m_RenderCommands;

		Ref<Shader> m_PBRShader;
		Ref<Shader> m_CompositeShader;
		Ref<Shader> m_SkyboxShader;

		Ref<FrameBuffer> m_GeometryBuffer;
		Ref<FrameBuffer> m_CompositeBuffer;

		Ref<Mesh> m_FullscreenQuad;

		Ref<Material> m_EnvironmentMaterial;

		Ref<Texture> m_BRDFLutTexture;

		byte* m_UniformBuffer;
		uint m_UniformBufferSize;

		std::unordered_map<RendererID, std::function<void(const Ref<ShaderResource>&, RenderCommand&, Ref<Shader>)>> m_RendererResourceFuncs;
		std::unordered_map<RendererID, std::function<void(const Ref<ShaderUniform>&, RenderCommand&, Ref<Shader>)>> m_RendererUniformFuncs;
	};

	static struct SceneRendererData s_Data;

	void SceneRenderer::Init()
	{
		InitRenderFunctions();
		InitRenderResourceFunFunctions();

		//Create buffer size as ratio of size of window
		// screen size App > Renderer > framebuffer
		// Create MSAA buffer
		// sample depth buffer
		// delete texture
		// texture needs to be a ref
		s_Data.m_GeometryBuffer = CreateRef<FrameBuffer>(1280, 720);
		s_Data.m_GeometryBuffer->Bind();
		s_Data.m_GeometryBuffer->Attach(BufferAtachment::COLOR);
		s_Data.m_GeometryBuffer->Attach(BufferAtachment::DEPTH);
		s_Data.m_GeometryBuffer->Unbind();

		s_Data.m_CompositeBuffer = CreateRef<FrameBuffer>(1280, 720, TextureParameters(TextureFormat::RGBA16F, TextureFilter::NEAREST, TextureWrap::CLAMP_TO_EDGE));
		s_Data.m_CompositeBuffer->Bind();
		s_Data.m_CompositeBuffer->Attach(BufferAtachment::COLOR);
		s_Data.m_CompositeBuffer->Attach(BufferAtachment::DEPTH);
		s_Data.m_CompositeBuffer->Unbind();

		s_Data.m_FullscreenQuad = MeshFactory::GenQuad(-1.0f, -1.0f, 0.0f, 2.0f, 2.0f); //create mesh cache
		s_Data.m_CompositeShader = CreateRef<Shader>("assets/shaders/HDR.shader");
		s_Data.m_PBRShader = CreateRef<Shader>("assets/shaders/DefaultPBR.shader");
		s_Data.m_SkyboxShader = CreateRef<Shader>("assets/shaders/Skybox.shader");

		s_Data.m_BRDFLutTexture = CreateRef<Texture>("assets/textures/Brdf_Lut.png");

		s_Data.m_EnvironmentMaterial = CreateRef<Material>(s_Data.m_SkyboxShader);
		s_Data.m_EnvironmentMaterial->SetDepthTesting(false);
	}

	void SceneRenderer::Begin(Scene* scene)
	{
		Renderer::Begin();

		s_Data.m_ActiveScene = scene;
		s_Data.m_EnvironmentMaterial->SetTexture("u_SkyboxTexture", s_Data.m_ActiveScene->GetEnvironmentMap()->irradianceMap);
		s_Data.m_EnvironmentMaterial->SetDepthTesting(false);
		s_Data.m_GeometryBuffer->Bind();
		s_Data.m_GeometryBuffer->Clear();
	}

	void SceneRenderer::Flush()
	{
		auto it = s_Data.m_RenderCommands.begin();
		s_Data.m_RenderCommands.insert(it, RenderCommand(s_Data.m_FullscreenQuad, glm::mat4(1.0f), s_Data.m_EnvironmentMaterial));

		for (int i = 0; i < s_Data.m_RenderCommands.size(); i++)
		{
			RenderCommand command = s_Data.m_RenderCommands.at(i);

			// Clear all textures
			for (uint32_t i = 0; i < 32; i++)
				glBindTextureUnit(i, 0);

			auto materials = command.mesh->GetMaterials();
		//	Ref<Material> material = materials.size() ? materials[0] : command.material;
			Ref<Material> material = command.material;

			material->Bind();
			Ref<Shader> shader = material->GetShader();

			//This needs to be cached
			std::vector<UniformBuffer*> uniformBuffers = shader->GetUniformBuffers(UniformSystemType::RENDERER);

			for (UniformBuffer* uniformBuffer : uniformBuffers)
			{
				std::vector<Ref<ShaderUniform>> uniforms = uniformBuffer->uniforms;
				
				s_Data.m_UniformBufferSize = uniformBuffer->size;
				s_Data.m_UniformBuffer = new byte[uniformBuffer->size];

				for (Ref<ShaderUniform> uniform : uniforms)
				{
					s_Data.m_RendererUniformFuncs[uniform->GetRendererID()](uniform, command, shader);
				}

				shader->UploadUniformBuffer(uniformBuffer->index, s_Data.m_UniformBuffer, s_Data.m_UniformBufferSize);
				delete s_Data.m_UniformBuffer;
			}

			std::vector<Ref<ShaderResource>> resources = shader->GetResources(UniformSystemType::RENDERER);
			for (Ref<ShaderResource> resource : resources)
			{
				RendererID rendererID = resource->GetRendererID();
				s_Data.m_RendererResourceFuncs[rendererID](resource, command, shader);
			}

			Renderer::SubmitMesh(command.mesh, command.transform, material, s_Data.m_ActiveScene);
		}

		s_Data.m_GeometryBuffer->Unbind();
		s_Data.m_RenderCommands.clear();

		Renderer::Flush();
	}

	void SceneRenderer::End()
	{
		s_Data.m_CompositeBuffer->Bind();
		s_Data.m_CompositeBuffer->Clear();

		s_Data.m_CompositeShader->Bind();
		s_Data.m_CompositeShader->SetUniform1f("r_Exposure", s_Data.m_ActiveScene->GetCamera()->GetExposure());
		s_Data.m_CompositeShader->SetUniform1i("r_InputTexture", 5);

		s_Data.m_GeometryBuffer->GetTexture()->Bind(5);

		s_Data.m_FullscreenQuad->Render(true);

		s_Data.m_CompositeBuffer->Unbind();

		Renderer::End();
	}

	void SceneRenderer::SubmitMesh(Ref<Mesh> mesh, glm::mat4 transform, Ref<Material> material)
	{
		if (material != nullptr)
			s_Data.m_RenderCommands.push_back(RenderCommand(mesh, transform, material));
	//	else if (mesh.HasMaterial())
	//		s_Data.m_RenderCommands.push_back(RenderCommand(mesh, transform, mesh.getMaterial()));
	}

	void SceneRenderer::Resize(uint width, uint height)
	{
		s_Data.m_CompositeBuffer->Resize(width, height);
		s_Data.m_GeometryBuffer->Resize(width, height);
	}

	Ref<Shader> SceneRenderer::GetPBRShader()
	{
		return s_Data.m_PBRShader;
	}

	Ref<FrameBuffer> SceneRenderer::GetFinalBuffer()
	{
		return s_Data.m_CompositeBuffer;
	}

	void SceneRenderer::InitRenderFunctions()
	{
		s_Data.m_RendererUniformFuncs[RendererID::MODEL_MATRIX] = [&](const Ref<ShaderUniform>& uniform, RenderCommand& command, Ref<Shader> shader)
		{
		//	shader->SetUniformMat4(uniform.Location, command.transform);
			memcpy(s_Data.m_UniformBuffer + uniform->GetOffset(), &command.transform, uniform->GetSize());
		//	s_Data.m_UniformBufferSize += sizeof(glm::mat4);
		};
		s_Data.m_RendererUniformFuncs[RendererID::VIEW_MATRIX] = [&](const Ref<ShaderUniform>& uniform, RenderCommand& command, Ref<Shader> shader)
		{
		//	shader->SetUniformMat4(uniform.Location, s_Data.m_ActiveScene->GetCamera()->GetViewMatrix());
			auto view = s_Data.m_ActiveScene->GetCamera()->GetViewMatrix();
			memcpy(s_Data.m_UniformBuffer + uniform->GetOffset(), glm::value_ptr(view), uniform->GetSize());
		//	s_Data.m_UniformBufferSize += sizeof(glm::mat4);
		};
		s_Data.m_RendererUniformFuncs[RendererID::PROJ_MATRIX] = [&](const Ref<ShaderUniform>& uniform, RenderCommand& command, Ref<Shader> shader)
		{
		//	shader->SetUniformMat4(uniform.Location, s_Data.m_ActiveScene->GetCamera()->GetProjectionMatrix());
			auto proj = s_Data.m_ActiveScene->GetCamera()->GetProjectionMatrix();
			memcpy(s_Data.m_UniformBuffer + uniform->GetOffset(), glm::value_ptr(proj), uniform->GetSize());
		//	s_Data.m_UniformBufferSize += sizeof(glm::mat4);
		};
		s_Data.m_RendererUniformFuncs[RendererID::INVERSE_VP] = [&](const Ref<ShaderUniform>& uniform, RenderCommand& command, Ref<Shader> shader)
		{
		//	shader->SetUniformMat4(uniform.Location, glm::inverse(s_Data.m_ActiveScene->GetCamera()->GetViewMatrix()));
			glm::mat4 ivp = glm::inverse(s_Data.m_ActiveScene->GetCamera()->GetViewMatrix() * s_Data.m_ActiveScene->GetCamera()->GetProjectionMatrix());
			memcpy(s_Data.m_UniformBuffer + uniform->GetOffset(), glm::value_ptr(ivp), uniform->GetSize());
		//	s_Data.m_UniformBufferSize += sizeof(glm::mat4);
		};
		s_Data.m_RendererUniformFuncs[RendererID::MVP] = [&](const Ref<ShaderUniform>& uniform, RenderCommand& command, Ref<Shader> shader)
		{
		//	shader->SetUniformMat4(uniform.Location, s_Data.m_ActiveScene->GetCamera()->GetProjectionMatrix() * s_Data.m_ActiveScene->GetCamera()->GetViewMatrix() * command.transform);
			glm::mat4 mvp = s_Data.m_ActiveScene->GetCamera()->GetProjectionMatrix() * s_Data.m_ActiveScene->GetCamera()->GetViewMatrix() * command.transform;
			memcpy(s_Data.m_UniformBuffer + uniform->GetOffset(), &mvp, uniform->GetSize());
		//	s_Data.m_UniformBufferSize += sizeof(glm::mat4);
		};
		s_Data.m_RendererUniformFuncs[RendererID::CAMERA_POSITION] = [&](const Ref<ShaderUniform>& uniform, RenderCommand& command, Ref<Shader> shader)
		{
		//	shader->SetUniform3f(uniform.Location, s_Data.m_ActiveScene->GetCamera()->GetPosition()); 
			auto pos = s_Data.m_ActiveScene->GetCamera()->GetPosition();
			memcpy(s_Data.m_UniformBuffer + uniform->GetOffset(), glm::value_ptr(pos), uniform->GetSize());
		//	s_Data.m_UniformBufferSize += sizeof(glm::vec3);
		};
	}
	void SceneRenderer::InitRenderResourceFunFunctions()
	{
		s_Data.m_RendererResourceFuncs[RendererID::IRRADIANCE_TEXTURE] = [&](const Ref<ShaderResource>& uniform, RenderCommand& command, Ref<Shader> shader)
		{
			s_Data.m_ActiveScene->GetEnvironmentMap()->irradianceMap->Bind(uniform->GetSampler());
		};
		s_Data.m_RendererResourceFuncs[RendererID::RADIANCE_TEXTURE] = [&](const Ref<ShaderResource>& uniform, RenderCommand& command, Ref<Shader> shader)
		{
			s_Data.m_ActiveScene->GetEnvironmentMap()->radianceMap->Bind(uniform->GetSampler());
		};
		s_Data.m_RendererResourceFuncs[RendererID::BRDF_LUT] = [&](const Ref<ShaderResource>& uniform, RenderCommand& command, Ref<Shader> shader)
		{
			s_Data.m_BRDFLutTexture->Bind(uniform->GetSampler());
		};
	}
}