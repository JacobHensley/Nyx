#include "NXpch.h"
#include "Renderer.h"
#include "Nyx/graphics/MeshFactory.h"

namespace Nyx {

	Renderer* Renderer::s_Instance = new Renderer();

	std::vector<RenderCommand> Renderer::m_Commands;

	Scene* Renderer::m_ActiveScene = nullptr;
	Shader* Renderer::m_HDRShader = nullptr;
	FrameBuffer* Renderer::m_HDRBuffer = nullptr;
	FrameBuffer* Renderer::m_FinalBuffer = nullptr;
	Mesh* Renderer::m_FullscreenQuad = nullptr;
	float Renderer::m_Exposure = 1.0f;

	Renderer::Renderer()
	{
		
	}

	Renderer::~Renderer()
	{
	}

	void Renderer::Begin(Scene* scene)
	{
		m_ActiveScene = scene;
		m_HDRBuffer->Clear();
		m_HDRBuffer->Bind();
	}

	void Renderer::Flush()
	{
		for (int i = 0;i < m_Commands.size();i++)
		{
			RenderCommand command = m_Commands.at(i);
			command.material->Bind();
			command.material->SetUniform("u_InverseVP", glm::inverse(m_ActiveScene->GetCamera()->GetViewMatrix()));
			command.material->SetUniform("u_CameraPosition", m_ActiveScene->GetCamera()->GetPosition());
			command.material->SetUniform("u_ModelMatrix", command.transform);
			command.material->SetUniform("u_MVP", m_ActiveScene->GetCamera()->GetProjectionMatrix() * m_ActiveScene->GetCamera()->GetViewMatrix() * command.transform);
			command.mesh->Render(command.depthTesting);
		}

		m_Commands.clear();
	}

	void Renderer::End()
	{
		m_HDRBuffer->Unbind();

		//Bind Renderspace buffer
		m_FinalBuffer->Clear();
		m_FinalBuffer->Bind();

		//Render HDR buffer to renderspace buffer
		m_HDRShader->Bind();
		m_HDRShader->SetUniform1i("u_InputTexture", 0);
		m_HDRShader->SetUniform1f("u_Exposure", m_Exposure);
		m_HDRBuffer->GetTexture()->Bind(0);
		m_FullscreenQuad->Render(true);
		m_HDRShader->Unbind();

		m_FinalBuffer->Unbind();
	}

	void Renderer::SubmitMesh(Mesh* mesh, glm::mat4 transform, Material* material, bool depthTesting)
	{
		m_Commands.push_back(RenderCommand(mesh, transform, material, depthTesting));
	}

	void Renderer::InitGL()
	{
		m_FullscreenQuad = MeshFactory::GenQuad(-1.0f, -1.0f, 0.0f, 2.0f, 2.0f);
		m_HDRShader = new Shader("assets/shaders/HDR.shader");
		m_HDRBuffer = new FrameBuffer(1280, 720, TextureParameters(TextureFormat::RGBA16F, TextureFilter::NEAREST, TextureWrap::CLAMP_TO_EDGE));
		m_FinalBuffer = new FrameBuffer(1280, 720);
	}

}