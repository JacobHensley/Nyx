#include "NXpch.h"
#include "SceneRenderer.h"
#include "Nyx/graphics/renderer/Renderer.h"
#include "Nyx/graphics/MeshFactory.h"
#include "glm/gtc/type_ptr.hpp"
#include "Nyx/AssetManager.h"

namespace Nyx {

	struct SceneRendererData
	{
		Scene* m_ActiveScene;
		std::vector<RenderCommand> m_RenderCommands;
		Ref<RenderPass> m_GeometryPass;
		Ref<RenderPass> m_CompositePass;
		Ref<Shader> m_CompositeShader;

		Ref<Mesh> m_FullscreenQuad;
		Ref<Shader> m_PBRShader;

		Ref<Material> m_EnvironmentMaterial;
		Ref<Shader> m_SkyboxShader;
	};

	static struct SceneRendererData s_Data;

	void SceneRenderer::Init()
	{
		//Create buffer size as ratio of size of window
		// screen size App > Renderer > framebuffer
		// Create MSAA buffer
		// sample depth buffer

		s_Data.m_GeometryPass = CreateRef<RenderPass>();
		s_Data.m_CompositePass = CreateRef<RenderPass>(TextureParameters(TextureFormat::RGBA16F, TextureFilter::NEAREST, TextureWrap::CLAMP_TO_EDGE));

		s_Data.m_FullscreenQuad = MeshFactory::GenQuad(-1.0f, -1.0f, 0.0f, 2.0f, 2.0f); //create mesh cache
		s_Data.m_CompositeShader = CreateRef<Shader>("assets/shaders/HDR.shader");
		s_Data.m_PBRShader = CreateRef<Shader>("assets/shaders/DefaultPBR.shader");

		s_Data.m_SkyboxShader = CreateRef<Shader>("assets/shaders/Skybox.shader");
		s_Data.m_EnvironmentMaterial = CreateRef<Material>(s_Data.m_SkyboxShader);
		s_Data.m_EnvironmentMaterial->SetDepthTesting(false);
	}

	void SceneRenderer::Begin(Scene* scene)
	{
		s_Data.m_ActiveScene = scene;

		s_Data.m_EnvironmentMaterial->SetTexture("u_SkyboxTexture", AssetManager::GetByUUID<TextureCube>(s_Data.m_ActiveScene->GetEnvironmentMap()->irradianceMap.GetUUID()));
	}

	void SceneRenderer::Flush()
	{
		GeometryPass();
		s_Data.m_RenderCommands.clear();
	}

	void SceneRenderer::End()
	{
		CompositePass();
	}

	void Nyx::SceneRenderer::SubmitMesh(Ref<Mesh> mesh, glm::mat4 transform)
	{
		s_Data.m_RenderCommands.push_back(RenderCommand(mesh, transform));
	}

	void Nyx::SceneRenderer::SubmitMesh(Ref<Mesh> mesh, glm::mat4 transform, Ref<Material> materialOverride)
	{
		s_Data.m_RenderCommands.push_back(RenderCommand(mesh, transform, materialOverride));
	}

	Ref<FrameBuffer> Nyx::SceneRenderer::GetFinalBuffer()
	{
		return s_Data.m_CompositePass->GetFrameBuffer();
	}

	void Nyx::SceneRenderer::Resize(uint width, uint height)
	{
		s_Data.m_CompositePass->GetFrameBuffer()->Resize(width, height);
		s_Data.m_GeometryPass->GetFrameBuffer()->Resize(width, height);
	}

	Ref<Shader> Nyx::SceneRenderer::GetPBRShader()
	{
		return s_Data.m_PBRShader;
	}

	void SceneRenderer::GeometryPass()
	{
		s_Data.m_GeometryPass->Bind();

		//Scene level sorting

		Renderer::SubmitMesh(s_Data.m_ActiveScene, s_Data.m_FullscreenQuad, glm::mat4(1.0f), s_Data.m_EnvironmentMaterial);

		for (RenderCommand command : s_Data.m_RenderCommands)
		{
			if (command.materialOverride == nullptr)
				Renderer::SubmitMesh(s_Data.m_ActiveScene, command.mesh, command.transform);
			else
				Renderer::SubmitMesh(s_Data.m_ActiveScene, command.mesh, command.transform, command.materialOverride);
		}

		s_Data.m_GeometryPass->Unbind();
	}

	void SceneRenderer::CompositePass()
	{
		s_Data.m_CompositePass->Bind();

		s_Data.m_CompositeShader->Bind();
		s_Data.m_CompositeShader->SetUniform1f("r_Exposure", *s_Data.m_ActiveScene->GetCamera()->GetExposure());
		s_Data.m_CompositeShader->SetUniform1f("r_ExposureActive", *s_Data.m_ActiveScene->GetCamera()->GetExposureActive());
		s_Data.m_CompositeShader->SetUniform1i("r_InputTexture", 5);

		s_Data.m_GeometryPass->GetFrameBuffer()->GetTexture()->Bind(5);

		s_Data.m_FullscreenQuad->Render(true);

		s_Data.m_CompositePass->Unbind();
	}

}