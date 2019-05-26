#pragma once
#include "Nyx/graphics/renderer/API/Texture.h"

namespace Nyx {

	class FrameBuffer
	{
	public:
		FrameBuffer(int width, int height, TextureParameters parameters = TextureParameters());
		~FrameBuffer();
		
		void Bind();
		void Unbind();

		inline uint GetFrameBufferID() const { return m_FrameBufferID; }
		inline uint GetRenderBufferID() const { return m_RenderBufferID; }
		inline const Texture* GetTexture() const { return m_Texture; }
	private:
		uint m_FrameBufferID;
		uint m_RenderBufferID;
		Texture* m_Texture;
	};

}