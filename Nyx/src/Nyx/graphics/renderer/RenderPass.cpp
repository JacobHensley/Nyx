#include "NXpch.h"
#include "RenderPass.h"
#include "Nyx/graphics/MeshFactory.h"

namespace Nyx 
{
	Nyx::RenderPass::RenderPass(Ref<FrameBuffer> targetBuffer)
		: m_TargetBuffer(targetBuffer)
	{
		m_FullscreenQuad = MeshFactory::GenQuad(-1.0f, -1.0f, 0.0f, 2.0f, 2.0f);
	}

	void Nyx::RenderPass::Bind()
	{
		m_TargetBuffer->Bind();
		m_TargetBuffer->Clear();
	}

	void Nyx::RenderPass::Unbind()
	{
		m_TargetBuffer->Unbind();
	}

	void RenderPass::SetLastRenderPass(Ref<RenderPass> pass)
	{
		m_TargetBuffer = pass->GetTargetBuffer();
	}

	void RenderPass::Render(Ref<Shader> shader)
	{
		Bind();
		shader->SetUniform1i("u_InputTexture", 0);
		m_TargetBuffer->GetTexture()->Bind(0);
		m_FullscreenQuad->Render(true);
		Unbind();
	}
}