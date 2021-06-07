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

		void BindColorTexture(uint32_t slot, uint32_t index);
		void BindDepthTexture(uint32_t slot);

		void Clear();
		void Resize(int width, int height);
		void SetViewPortSize(int x, int y, int width, int height);

		inline uint32_t GetFrameBufferID() const { return m_FrameBufferID; }
		inline std::vector<uint32_t>& GetColorAttachments() { return m_ColorAttachments; }
		inline uint32_t const GetDepthAttachment() const { return m_DepthAttachment; }

		inline uint32_t GetWidth() const { return m_Width;}
		inline uint32_t GetHeight() const { return m_Height; }

	private:
		bool m_HasColorBuffer = false;
		bool m_HasDepthBuffer = false;

		uint32_t m_FrameBufferID = 0;
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;

		FramebufferSpecification m_Specification;
		TextureParameters m_Parameters;

		std::vector<uint32_t> m_ColorAttachments;
		uint32_t m_DepthAttachment = 0;
	};

}