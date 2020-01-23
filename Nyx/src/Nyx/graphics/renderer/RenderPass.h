#pragma once
#include "Nyx/graphics/renderer/API/FrameBuffer.h"
#include "Nyx/graphics/renderer/Shader.h"

namespace Nyx {

	class RenderPass
	{
	public:
		RenderPass(FrameBuffer* framebuffer, Shader* shader);

	public:
		void Bind();
		void Unbind();

		void Render();

		inline FrameBuffer* GetFramebuffer() { return m_Framebuffer; }
		inline Shader* GetShader() { return m_Shader; }
	private:
		FrameBuffer* m_Framebuffer = nullptr;
		Shader* m_Shader = nullptr;
		Mesh* m_FullscreenQuad;
	};

}