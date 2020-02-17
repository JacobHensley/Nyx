#include "NXpch.h"
#include "SceneRenderer.h"
#include "Nyx/graphics/renderer/Renderer.h"

namespace Nyx {

	SceneRenderer* SceneRenderer::s_Instance = new SceneRenderer();

	SceneRenderer::SceneRenderer()
	{
		m_RendererUniformFuncs[RenderUniformID::MODEL_MATRIX] = [&](const RendererUniform& uniform, RenderCommand& command, Shader* shader)
		{
			shader->SetUniformMat4(uniform.Location, command.transform);
		};
		m_RendererUniformFuncs[RenderUniformID::VIEW_MATRIX] = [&](const RendererUniform& uniform, RenderCommand& command, Shader* shader)
		{
			shader->SetUniformMat4(uniform.Location, m_ActiveScene->GetCamera()->GetViewMatrix());
		};
		m_RendererUniformFuncs[RenderUniformID::PROJ_MATRIX] = [&](const RendererUniform& uniform, RenderCommand& command, Shader* shader)
		{
			shader->SetUniformMat4(uniform.Location, m_ActiveScene->GetCamera()->GetProjectionMatrix());
		};
		m_RendererUniformFuncs[RenderUniformID::INVERSE_VP] = [&](const RendererUniform& uniform, RenderCommand& command, Shader* shader)
		{
			shader->SetUniformMat4(uniform.Location, glm::inverse(m_ActiveScene->GetCamera()->GetViewMatrix()));
		};
		m_RendererUniformFuncs[RenderUniformID::MVP] = [&](const RendererUniform& uniform, RenderCommand& command, Shader* shader)
		{
			shader->SetUniformMat4(uniform.Location, m_ActiveScene->GetCamera()->GetProjectionMatrix() * m_ActiveScene->GetCamera()->GetViewMatrix() * command.transform);
		};
		m_RendererUniformFuncs[RenderUniformID::CAMERA_POSITION] = [&](const RendererUniform& uniform, RenderCommand& command, Shader* shader)
		{
			shader->SetUniform3f(uniform.Location, m_ActiveScene->GetCamera()->GetPosition());
		};
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
		// Set Camera

		for (int i = 0; i < m_RenderCommands.size(); i++)
		{
			RenderCommand command = m_RenderCommands.at(i);
			command.material->Bind();
			Shader* shader = command.material->GetShader();

			// Transform

			const auto& rendererUniforms = shader->GetRendererUniforms();
			for (const auto& uniform : rendererUniforms)
				m_RendererUniformFuncs[uniform.ID](uniform, command, shader);

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