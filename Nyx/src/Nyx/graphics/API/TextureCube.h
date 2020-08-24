#pragma once
#include "Nyx/Common.h"
#include "Nyx/Asset.h"

namespace Nyx {

	class TextureCube : public Asset
	{
	public:
		TextureCube(const String& path);
		~TextureCube();

	public:
		void Bind(uint slot);
		void Unbind();

		inline const String& GetPath() { return m_Path; }
		inline const uint GetTextureID() const { return m_TextureID; }

	private:
		uint LoadFromFile(const String& path);

		String m_Path;
		uint m_TextureID;
		int m_Width;
		int m_Height;
	};

}