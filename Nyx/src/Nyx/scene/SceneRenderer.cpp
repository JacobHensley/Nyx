#include "NXpch.h"
#include "SceneRenderer.h"
#include "Nyx/graphics/renderer/Renderer.h"
#include "Nyx/graphics/MeshFactory.h"

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

		std::unordered_map<RenderUniformID, std::function<void(const RendererUniform&, RenderCommand&, Ref<Shader>)>> m_RendererUniformFuncs;
	};

	static struct SceneRendererData s_Data;

	void SceneRenderer::Init()
	{
		InitRenderFunctions();

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

		s_Data.m_FullscreenQuad = MeshFactory::GenQuad(-1.0f, -1.0f, 0.0f, 2.0f, 2.0f);
		s_Data.m_CompositeShader = CreateRef<Shader>("assets/shaders/HDR.shader");
		s_Data.m_PBRShader = CreateRef<Shader>("assets/shaders/DefaultPBR.shader");
		s_Data.m_SkyboxShader = CreateRef<Shader>("assets/shaders/Skybox.shader");

		s_Data.m_EnvironmentMaterial = CreateRef<Material>(s_Data.m_SkyboxShader);
		s_Data.m_EnvironmentMaterial->SetDepthTesting(false);
	}

	void SceneRenderer::Begin(Scene* scene)
	{
		Renderer::Begin();

		s_Data.m_ActiveScene = scene;

		s_Data.m_EnvironmentMaterial->SetTexture("u_SkyboxTexture", s_Data.m_ActiveScene->GetEnvironmentMap()->irradianceMap);
		
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

			auto materials = command.mesh->GetMaterials();
			Ref<Material> material = materials.size() ? materials[0] : command.material;
		//	Ref<Material> material = command.material;

			material->Bind();
			Ref<Shader> shader = material->GetShader();

			const auto& rendererUniforms = shader->GetRendererUniforms();
			for (const auto& uniform : rendererUniforms)
				s_Data.m_RendererUniformFuncs[uniform.ID](uniform, command, shader);

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
		s_Data.m_CompositeShader->SetUniform1f("u_Exposure", s_Data.m_ActiveScene->GetCamera()->GetExposure());
		s_Data.m_CompositeShader->SetUniform1i("u_InputTexture", 0);

		s_Data.m_GeometryBuffer->GetTexture()->Bind(0);

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
		s_Data.m_RendererUniformFuncs[RenderUniformID::MODEL_MATRIX] = [&](const RendererUniform& uniform, RenderCommand& command, Ref<Shader> shader)
		{
			shader->SetUniformMat4(uniform.Location, command.transform);
		};
		s_Data.m_RendererUniformFuncs[RenderUniformID::VIEW_MATRIX] = [&](const RendererUniform& uniform, RenderCommand& command, Ref<Shader> shader)
		{
			shader->SetUniformMat4(uniform.Location, s_Data.m_ActiveScene->GetCamera()->GetViewMatrix());
		};
		s_Data.m_RendererUniformFuncs[RenderUniformID::PROJ_MATRIX] = [&](const RendererUniform& uniform, RenderCommand& command, Ref<Shader> shader)
		{
			shader->SetUniformMat4(uniform.Location, s_Data.m_ActiveScene->GetCamera()->GetProjectionMatrix());
		};
		s_Data.m_RendererUniformFuncs[RenderUniformID::INVERSE_VP] = [&](const RendererUniform& uniform, RenderCommand& command, Ref<Shader> shader)
		{
			shader->SetUniformMat4(uniform.Location, glm::inverse(s_Data.m_ActiveScene->GetCamera()->GetViewMatrix()));
		};
		s_Data.m_RendererUniformFuncs[RenderUniformID::MVP] = [&](const RendererUniform& uniform, RenderCommand& command, Ref<Shader> shader)
		{
			shader->SetUniformMat4(uniform.Location, s_Data.m_ActiveScene->GetCamera()->GetProjectionMatrix() * s_Data.m_ActiveScene->GetCamera()->GetViewMatrix() * command.transform);
		};
		s_Data.m_RendererUniformFuncs[RenderUniformID::CAMERA_POSITION] = [&](const RendererUniform& uniform, RenderCommand& command, Ref<Shader> shader)
		{
			shader->SetUniform3f(uniform.Location, s_Data.m_ActiveScene->GetCamera()->GetPosition());
		};
	}
}