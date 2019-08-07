#pragma once
#include "Nyx/Common.h"

namespace Nyx {

	class TextureCube
	{
	public:
		TextureCube(const String& path);
		~TextureCube();

		inline const uint GetTextureID() const { return m_TextureID; }
		void bind();
		void unbind();
	private:
		uint LoadFromFile(const String& path);

		String m_Path;
		uint m_TextureID;
		int m_Width;
		int m_Height;
	};

}