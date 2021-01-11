#pragma once
#include "Nyx/Core/Core.h"
#include "Nyx/Asset/Asset.h"
#include <String>

namespace Nyx {

	class TextureCube : public Asset
	{
	public:
		TextureCube(const std::string& path);
		~TextureCube();

	public:
		void Bind(uint slot);
		void Unbind();

		inline const std::string& GetPath() { return m_Path; }
		inline const uint GetTextureID() const { return m_TextureID; }

	private:
		uint LoadFromFile(const std::string& path);

		std::string m_Path;
		uint m_TextureID;
		int m_Width;
		int m_Height;
	};

}