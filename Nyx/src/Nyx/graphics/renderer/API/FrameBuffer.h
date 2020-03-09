#pragma once
#include "Nyx/graphics/renderer/API/Texture.h"

namespace Nyx {

	enum BufferAtachment
	{
		COLOR, DEPTH
	};

	class FrameBuffer
	{
	public:
		FrameBuffer(int width, int height, TextureParameters parameters = TextureParameters());
		~FrameBuffer();
		
	public:
		void Bind();
		void Unbind();

		void Attach(BufferAtachment attachment);

		void Clear();
		void Resize(int width, int height);
		void SetViewPortSize(int x, int y, int width, int height);

		inline Texture* GetTexture() const { return m_Texture; }
		inline uint GetFrameBufferID() const { return m_FrameBufferID; }
		inline uint GetRenderBufferID() const { return m_RenderBufferID; }

	private:
		bool m_HasColorBuffer = false;
		bool m_HasDepthBuffer = false;

		uint m_FrameBufferID = 0;
		uint m_RenderBufferID = 0;

		Texture* m_Texture = nullptr;
		TextureParameters m_Parameters;
	};

}