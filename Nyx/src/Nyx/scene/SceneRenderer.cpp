#include "NXpch.h"
#include "SceneRenderer.h"
#include "Nyx/graphics/renderer/Renderer.h"
#include "Nyx/graphics/MeshFactory.h"

namespace Nyx {

	SceneRenderer* SceneRenderer::s_Instance = new SceneRenderer();

	SceneRenderer::SceneRenderer()
	{
		m_RendererUniformFuncs[RenderUniformID::MODEL_MATRIX] = [&](const RendererUniform& uniform, RenderCommand& command, Ref<Shader> shader)
		{
			shader->SetUniformMat4(uniform.Location, command.transform);
		};
		m_RendererUniformFuncs[RenderUniformID::VIEW_MATRIX] = [&](const RendererUniform& uniform, RenderCommand& command, Ref<Shader> shader)
		{
			shader->SetUniformMat4(uniform.Location, m_ActiveScene->GetCamera()->GetViewMatrix());
		};
		m_RendererUniformFuncs[RenderUniformID::PROJ_MATRIX] = [&](const RendererUniform& uniform, RenderCommand& command, Ref<Shader> shader)
		{
			shader->SetUniformMat4(uniform.Location, m_ActiveScene->GetCamera()->GetProjectionMatrix());
		};
		m_RendererUniformFuncs[RenderUniformID::INVERSE_VP] = [&](const RendererUniform& uniform, RenderCommand& command, Ref<Shader> shader)
		{
			shader->SetUniformMat4(uniform.Location, glm::inverse(m_ActiveScene->GetCamera()->GetViewMatrix()));
		};
		m_RendererUniformFuncs[RenderUniformID::MVP] = [&](const RendererUniform& uniform, RenderCommand& command, Ref<Shader> shader)
		{
			shader->SetUniformMat4(uniform.Location, m_ActiveScene->GetCamera()->GetProjectionMatrix() * m_ActiveScene->GetCamera()->GetViewMatrix() * command.transform);
		};
		m_RendererUniformFuncs[RenderUniformID::CAMERA_POSITION] = [&](const RendererUniform& uniform, RenderCommand& command, Ref<Shader> shader)
		{
			shader->SetUniform3f(uniform.Location, m_ActiveScene->GetCamera()->GetPosition());
		};
	}

	SceneRenderer::~SceneRenderer()
	{
	}

	void SceneRenderer::BeginI(Scene* scene)
	{
		Renderer::Begin();
		m_ActiveScene = scene;

		m_GeometryBuffer->Bind();
		m_GeometryBuffer->Clear();

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

			auto materials = command.mesh->GetMaterials();
			Ref<Material> material = materials.size() ? materials[0] : command.material;

			material->Bind();
			Ref<Shader> shader = material->GetShader();

			// Transform
			const auto& rendererUniforms = shader->GetRendererUniforms();
			for (const auto& uniform : rendererUniforms)
				m_RendererUniformFuncs[uniform.ID](uniform, command, shader);

			Renderer::SubmitMesh(command.mesh, command.transform, material, m_ActiveScene);
		}

		m_GeometryBuffer->Unbind();

		m_RenderCommands.clear();

		Renderer::Flush();
	}

	void SceneRenderer::EndI()
	{
		m_CompositeBuffer->Bind();
		m_CompositeBuffer->Clear();

		m_CompositeShader->Bind();
		m_CompositeShader->SetUniform1f("u_Exposure", m_Exposure);
		m_CompositeShader->SetUniform1i("u_InputTexture", 0);
		m_GeometryBuffer->GetTexture()->Bind(0);
		m_FullscreenQuad->Render(true);

		m_CompositeBuffer->Unbind();

		Renderer::End();
	}

	void SceneRenderer::ResizeI(uint32_t width, uint32_t height)
	{
		m_CompositeBuffer->Resize(width, height);
		m_GeometryBuffer->Resize(width, height);
	}

	void SceneRenderer::SubmitMeshI(Ref<Mesh> mesh, glm::mat4 transform, Ref<Material> material)
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
		m_GeometryBuffer = CreateRef<FrameBuffer>(1280, 720);
		m_GeometryBuffer->Bind();
		m_GeometryBuffer->Attach(BufferAtachment::COLOR);
		m_GeometryBuffer->Attach(BufferAtachment::DEPTH);
		m_GeometryBuffer->Unbind();

		m_CompositeBuffer = CreateRef<FrameBuffer>(1280, 720, TextureParameters(TextureFormat::RGBA16F, TextureFilter::NEAREST, TextureWrap::CLAMP_TO_EDGE));
		m_CompositeBuffer->Bind();
		m_CompositeBuffer->Attach(BufferAtachment::COLOR);
		m_CompositeBuffer->Attach(BufferAtachment::DEPTH);
		m_CompositeBuffer->Unbind();

		m_FullscreenQuad = MeshFactory::GenQuad(-1.0f, -1.0f, 0.0f, 2.0f, 2.0f);
		m_CompositeShader = CreateRef<Shader>("assets/shaders/HDR.shader");
		m_PBRShader = CreateRef<Shader>("assets/shaders/DefaultPBR.shader");
	}
}