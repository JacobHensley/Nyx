#include "NXpch.h"
#include "SceneRenderer.h"
#include "Nyx/graphics/renderer/Renderer.h"

namespace Nyx {

	SceneRenderer* SceneRenderer::s_Instance = new SceneRenderer();

	SceneRenderer::SceneRenderer()
	{
	}

	SceneRenderer::~SceneRenderer()
	{
	}

	void SceneRenderer::BeginI(Scene* scene)
	{
		Renderer::Begin(scene);
		m_ActiveScene = scene;

		m_HDRPass->Bind();

		auto lights = m_ActiveScene->GetLightEnvironment()->GetLights();

		for (int i = 0; i < lights.size(); i++)
		{
		}
	}

	void SceneRenderer::FlushI()
	{
		for (int i = 0; i < m_RenderCommands.size(); i++)
		{
			RenderCommand command = m_RenderCommands.at(i);
			command.material->Bind();

			for (int j = 0; j < command.material->GetShader()->GetRenderUniformIDs().size(); j++)
			{
				RenderUniformID ID = command.material->GetShader()->GetRenderUniformIDs().at(j);
				Shader* shader = command.material->GetShader();
				if (ID == RenderUniformID::MODEL_MATRIX)
					shader->SetUniformMat4("r_ModelMatrix", command.transform);
				if (ID == RenderUniformID::VIEW_MATRIX)
					shader->SetUniformMat4("r_ViewMatrix", m_ActiveScene->GetCamera()->GetViewMatrix());
				if (ID == RenderUniformID::PROJ_MATRIX)
					shader->SetUniformMat4("r_ProjMatrix", m_ActiveScene->GetCamera()->GetProjectionMatrix());
				if (ID == RenderUniformID::INVERSE_VP)
					shader->SetUniformMat4("r_InverseVP", glm::inverse(m_ActiveScene->GetCamera()->GetViewMatrix()));
				if (ID == RenderUniformID::MVP)
					shader->SetUniformMat4("r_MVP", m_ActiveScene->GetCamera()->GetProjectionMatrix() * m_ActiveScene->GetCamera()->GetViewMatrix() * command.transform);
				if (ID == RenderUniformID::CAMERA_POSITION)
					shader->SetUniform3f("r_CameraPosition", m_ActiveScene->GetCamera()->GetPosition());
			}

			Renderer::SubmitMesh(command.mesh, command.transform, command.material);
		}

		m_RenderCommands.clear();

		Renderer::Flush();
	}

	void SceneRenderer::EndI()
	{
		m_HDRPass->Unbind();

		m_FinalBuffer->Clear();
		m_FinalBuffer->Bind();

		m_HDRPass->GetShader()->Bind();
		m_HDRPass->GetShader()->SetUniform1f("u_Exposure", m_Exposure);
		m_HDRPass->Render();
		
		m_FinalBuffer->Unbind();

		Renderer::End();
	}

	void SceneRenderer::SubmitMeshI(Mesh* mesh, glm::mat4 transform, Material* material)
	{
		if (m_ActiveSceneMaterial != nullptr)
			m_RenderCommands.push_back(RenderCommand(mesh, transform, m_ActiveSceneMaterial));
		else if (material != nullptr)
			m_RenderCommands.push_back(RenderCommand(mesh, transform, material));
		//	else if (mesh->HasMaterial())
		//		m_RenderCommands.push_back(RenderCommand(mesh, transform, mesh->GetMaterial()));
	}

	void SceneRenderer::InitGLI()
	{
		m_FinalBuffer = new FrameBuffer(1280, 720);
		m_HDRFrameBuffer = new FrameBuffer(1280, 720, TextureParameters(TextureFormat::RGBA16F, TextureFilter::NEAREST, TextureWrap::CLAMP_TO_EDGE));
		m_HDRShader = new Shader("assets/shaders/HDR.shader");
		m_HDRPass = new RenderPass(m_HDRFrameBuffer, m_HDRShader);
	}


}