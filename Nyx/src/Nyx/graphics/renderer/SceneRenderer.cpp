#include "NXpch.h"
#include "SceneRenderer.h"
#include "Nyx/graphics/renderer/Renderer.h"
#include "Nyx/graphics/MeshFactory.h"
#include "glm/gtc/type_ptr.hpp"
#include "Nyx/AssetManager.h"
#include <glad/glad.h>

namespace Nyx {

	struct SceneRendererData
	{
		Scene* m_ActiveScene;
		Ref<Camera> m_ActiveCamera;


		std::vector<RenderCommand> m_RenderCommands;
		Ref<RenderPass> m_GeometryPass;
		Ref<RenderPass> m_CompositePass;
		Ref<Shader> m_CompositeShader;

		Ref<FrameBuffer> m_TempBuffer0;
		Ref<FrameBuffer> m_TempBuffer1;

		Ref<FrameBuffer> m_JumpFloodBuffer;
		Ref<FrameBuffer> m_SelectedObjectBuffer;

		Ref<Shader> m_JumpFloodInitShader;
		Ref<Shader> m_JumpFloodShader;
		Ref<Shader> m_FinalJumpFloodShader;
		Ref<Shader> m_CopyShader;

		Ref<Mesh> m_FullscreenQuad;
		Ref<Shader> m_PBRShader;

		Ref<Material> m_EnvironmentMaterial;
		Ref<Shader> m_SkyboxShader;
	};

	static struct SceneRendererData s_Data;

	void SceneRenderer::Init()
	{
		TextureParameters params;
		params.format = TextureFormat::RGBA16F;

		FramebufferSpecification fbSpec;
		fbSpec.Scale = 1.0f;
		fbSpec.ColorAttachments.emplace_back(params);
		fbSpec.DepthAttachments.emplace_back(params);
		s_Data.m_TempBuffer1 = CreateRef<FrameBuffer>(fbSpec);

		s_Data.m_GeometryPass = CreateRef<RenderPass>();
		s_Data.m_CompositePass = CreateRef<RenderPass>(TextureParameters(TextureFormat::RGBA, TextureFilter::LINEAR, TextureWrap::CLAMP_TO_EDGE));

		s_Data.m_FullscreenQuad = MeshFactory::GenQuad(-1.0f, -1.0f, 0.0f, 2.0f, 2.0f); //create mesh cache
		s_Data.m_CompositeShader = CreateRef<Shader>("assets/shaders/HDR.shader");
		s_Data.m_PBRShader = CreateRef<Shader>("assets/shaders/DefaultPBR.shader");

		s_Data.m_SkyboxShader = CreateRef<Shader>("assets/shaders/Skybox.shader");
		s_Data.m_EnvironmentMaterial = CreateRef<Material>(s_Data.m_SkyboxShader);
		s_Data.m_EnvironmentMaterial->SetDepthTesting(false);


		s_Data.m_TempBuffer0 = CreateRef<FrameBuffer>(fbSpec);
		s_Data.m_TempBuffer1 = CreateRef<FrameBuffer>(fbSpec);
		s_Data.m_JumpFloodBuffer = CreateRef<FrameBuffer>(fbSpec);

		s_Data.m_JumpFloodInitShader = CreateRef<Shader>("assets/shaders/JumpFloodInit.shader");
		s_Data.m_JumpFloodShader = CreateRef<Shader>("assets/shaders/JumpFlood.shader");
		s_Data.m_FinalJumpFloodShader = CreateRef<Shader>("assets/shaders/FinalJumpFlood.shader");
		s_Data.m_CopyShader = CreateRef<Shader>("assets/shaders/CopyShader.shader");
	}

	void SceneRenderer::Begin(Scene* scene, Ref<Camera> camera)
	{
		s_Data.m_ActiveScene = scene;
		s_Data.m_ActiveCamera = camera;

		s_Data.m_EnvironmentMaterial->SetTexture("u_SkyboxTexture", AssetManager::GetByUUID<TextureCube>(s_Data.m_ActiveScene->GetEnvironmentMap()->irradianceMap.GetUUID()));
	}

	void SceneRenderer::Flush()
	{
		GeometryPass();
		s_Data.m_RenderCommands.clear();

		JumpFloodPass();
		CompositePass();

		glDepthMask(GL_TRUE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	void SceneRenderer::End()
	{
	}

	void Nyx::SceneRenderer::SubmitMesh(Ref<Mesh> mesh, glm::mat4 transform)
	{
		s_Data.m_RenderCommands.push_back(RenderCommand(mesh, transform));
	}

	void Nyx::SceneRenderer::SubmitMesh(Ref<Mesh> mesh, glm::mat4 transform, Ref<Material> materialOverride)
	{
		s_Data.m_RenderCommands.push_back(RenderCommand(mesh, transform, materialOverride));
	}

	void SceneRenderer::SubmitSelectedMesh(Ref<Mesh> mesh, glm::mat4 transform, Ref<Material> materialOverride)
	{

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
		Renderer::Begin(s_Data.m_ActiveCamera);

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

	static int steps = 2;

	void SceneRenderer::JumpFloodPass()
	{
		steps = std::clamp(steps, 0, 32);

		glBlendFunc(GL_ONE, GL_ZERO);

		s_Data.m_JumpFloodInitShader->Bind();
		s_Data.m_JumpFloodInitShader->SetUniform2f("u_TextureSize", glm::vec2(1.0f / 1280.0f, 1.0f / 720.0f));
		Blit(s_Data.m_GeometryPass->GetFrameBuffer(), s_Data.m_TempBuffer0, s_Data.m_JumpFloodInitShader, true);

		int step = std::round(std::pow(steps - 1, 2));
		while (step != 0) {
			
			s_Data.m_JumpFloodShader->Bind();
			s_Data.m_JumpFloodShader->SetUniform1i("u_Step", step);
			s_Data.m_JumpFloodShader->SetUniform2f("u_TexelSize", glm::vec2(1.0f / 1280.0f, 1.0f / 720.0f));
			Blit(s_Data.m_TempBuffer0, s_Data.m_TempBuffer1, s_Data.m_JumpFloodShader, true);

			Ref<FrameBuffer> tmp = s_Data.m_TempBuffer0;
			s_Data.m_TempBuffer0 = s_Data.m_TempBuffer1;
			s_Data.m_TempBuffer1 = tmp;

			step /= 2;
		}

	}

	void SceneRenderer::Blit(Ref<FrameBuffer>& src, Ref<FrameBuffer>& dest, Ref<Shader>& shader, bool clear = true)
	{
		dest->Bind();
		if (clear)
			dest->Clear();

		shader->Bind();
		shader->SetUniform1i("r_InputTexture", 5);

		glBindTextureUnit(5, src->GetColorAttachments()[0]);

		s_Data.m_FullscreenQuad->Render(true);

		shader->Unbind();
		dest->Unbind();
	}

	void SceneRenderer::CompositePass()
	{
		s_Data.m_ActiveCamera->SetExposure(0.2f);

		s_Data.m_CompositePass->Bind();

		s_Data.m_CompositeShader->Bind();
		s_Data.m_CompositeShader->SetUniform1f("r_Exposure", *s_Data.m_ActiveCamera->GetExposure());
		s_Data.m_CompositeShader->SetUniform1i("r_InputTexture", 5);

		glBindTextureUnit(5, s_Data.m_GeometryPass->GetFrameBuffer()->GetColorAttachments()[0]);

		s_Data.m_FullscreenQuad->Render(true);

		s_Data.m_CompositePass->Unbind();

		glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		Blit(s_Data.m_TempBuffer0, s_Data.m_CompositePass->GetFrameBuffer(), s_Data.m_FinalJumpFloodShader, false);
		glEnable(GL_DEPTH_TEST);
	}

}