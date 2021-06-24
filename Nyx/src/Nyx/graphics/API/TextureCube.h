#pragma once
#include "Nyx/Core/Core.h"
#include "Nyx/Asset/Asset.h"
#include <String>

namespace Nyx {

	class TextureCube : public Asset
	{
	public:
		TextureCube(const std::string& path);
		TextureCube(uint32_t id, uint32_t width, uint32_t height);
		~TextureCube();

	public:
		void Bind(uint32_t slot);
		void Unbind();

		Ref<TextureCube> CalculateIrradianceMap();

		inline const std::string& GetPath() { return m_Path; }
		inline const uint32_t GetTextureID() const { return m_TextureID; }

	private:
		void LoadFromFile(const std::string& path);

		std::string m_Path;
		uint32_t m_TextureID;
		int m_Width;
		int m_Height;
	};

}