#pragma once
#include "Nyx/Core/Core.h"
#include "Nyx/Graphics/API/Texture.h"
#include <glm/glm.hpp>
#include <vector>

namespace Nyx {

	enum BufferAtachment
	{
		COLOR, DEPTH
	};

	struct FramebufferSpecification
	{
		float Scale = 1.0f;
		uint32_t Width = 0;
		uint32_t Height = 0;
		uint32_t Samples = 1; // multi-sampling

		glm::vec4 ClearColor = { 0.0f, 0.0f, 0.0f, 1.0f };

		std::vector<TextureParameters> ColorAttachments;
		std::vector<TextureParameters> DepthAttachments;

		bool SwapChainTarget = false;
	};

	class FrameBuffer
	{
	public:
		FrameBuffer(int width, int height, TextureParameters parameters = TextureParameters());
		FrameBuffer(const FramebufferSpecification& spec);
		~FrameBuffer();
		
	public:
		void Bind();
		void Unbind();

		void Attach(BufferAtachment attachment);

		void Clear();
		void Resize(int width, int height);
		void SetViewPortSize(int x, int y, int width, int height);

		inline Ref<Texture> GetTexture() const { return m_Texture; }
		inline uint GetFrameBufferID() const { return m_FrameBufferID; }
		inline std::vector<uint>& const GetColorAttachments() { return m_ColorAttachments; }

	private:
		bool m_HasColorBuffer = false;
		bool m_HasDepthBuffer = false;

		uint32_t m_Width = 0, m_Height = 0;

		uint m_FrameBufferID = 0;

		FramebufferSpecification m_Specification;
		std::vector<uint> m_ColorAttachments;
		uint m_DepthAttachment = 0;

		Ref<Texture> m_Texture = nullptr;
		TextureParameters m_Parameters;
	};

}