#include "NXpch.h"
#include "RenderPass.h"
#include "Nyx/graphics/MeshFactory.h"

namespace Nyx 
{
	Nyx::RenderPass::RenderPass(FrameBuffer* framebuffer, Shader* shader)
		: m_Framebuffer(framebuffer), m_Shader(shader)
	{
		m_FullscreenQuad = MeshFactory::GenQuad(-1.0f, -1.0f, 0.0f, 2.0f, 2.0f);
	}

	void Nyx::RenderPass::Bind()
	{
		m_Framebuffer->Clear();
		m_Framebuffer->Bind();
	}

	void Nyx::RenderPass::Unbind()
	{
		m_Framebuffer->Unbind();
	}

	void Nyx::RenderPass::Render()
	{
		m_Shader->Bind();
		m_Shader->SetUniform1i("u_InputTexture", 0);
		m_Framebuffer->GetTexture()->Bind(0);
		m_FullscreenQuad->Render(true);
		m_Shader->Unbind();
	}
}