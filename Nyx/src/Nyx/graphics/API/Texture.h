#pragma once
#include "Nyx/Core/Core.h"
#include "Nyx/Asset/Asset.h"
#include <String>

namespace Nyx {

	enum class TextureWrap
	{
		NONE = 0,
		REPEAT,
		MIRRORED_REPEAT,
		CLAMP_TO_EDGE,
		CLAMP_TO_BORDER
	};

	enum class TextureFilter
	{
		NONE = 0,
		LINEAR_MIPMAP,
		LINEAR,
		NEAREST
	};

	enum class TextureFormat
	{
		NONE = 0,
		RED,
		RGB,
		RGBA,
		RGBA16F,
		RGBA32F,
		LUMINANCE,
		LUMINANCE_ALPHA,
		DEPTH32F
	};

	struct TextureParameters
	{
		TextureFormat format;
		TextureFilter filter;
		TextureWrap wrap;
		bool generateMips = false;

		TextureParameters(TextureFormat format = TextureFormat::RGBA, TextureFilter filter = TextureFilter::LINEAR_MIPMAP, TextureWrap wrap = TextureWrap::REPEAT)
			: format(format), filter(filter), wrap(wrap)
		{
		}
	};

	class Texture : public Asset
	{
	public:
		Texture(const std::string& path, TextureParameters parameters = TextureParameters());
		Texture(int width, int height, TextureParameters parameters = TextureParameters());
		~Texture();

	public:
		void Bind(uint slot);
		void Unbind();

		void SetData(byte* imageData, uint size);

		bool Reload(const std::string& path);

		inline const int GetWidth() const { return m_Width; }
		inline const int GetHeight() const { return m_Height; }

		inline const uint GetTextureID() const { return m_TextureID; }
		inline const std::string& GetPath() const { return m_Path; }

	private:
		std::string m_Path;
		int m_Width, m_Height;
		TextureParameters m_Parameters;
		uint m_TextureID;

		uint LoadFromFile(const std::string& path);
		uint Init(byte* imageData, uint width, uint height);

		byte GetStrideFromFormat(TextureFormat format);

		static uint TextureFormatToGL(TextureFormat format);
		static uint TextureWrapToGL(TextureWrap wrap);
		static uint TextureFilterToGL(TextureFilter filter);
	};

}