#pragma once
#include "Nyx/graphics/renderer/Shader.h"
#include "Nyx/graphics/renderer/API/FrameBuffer.h"

namespace Nyx {

	class RenderPass
	{
	public:
		RenderPass(Ref<FrameBuffer> targetBuffer);

	public:
		void Bind();
		void Unbind();

		void SetLastRenderPass(Ref<RenderPass> pass);

		void Render(Ref<Shader> shader);

		inline void Resize(int width, int height) { m_TargetBuffer->Resize(width, height); }
		inline Ref<FrameBuffer> GetTargetBuffer() { return m_TargetBuffer; }
	private:
		Ref<Mesh> m_FullscreenQuad = nullptr;
		Ref<FrameBuffer> m_TargetBuffer = nullptr;
	};

}