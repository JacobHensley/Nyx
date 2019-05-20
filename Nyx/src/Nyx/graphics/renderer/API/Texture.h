#pragma once
#include "Nyx/Common.h"

namespace Nyx {

	enum class TextureWrap
	{
		NONE = 0,
		REPEAT,
		CLAMP,
		MIRRORED_REPEAT,
		CLAMP_TO_EDGE,
		CLAMP_TO_BORDER
	};

	enum class TextureFilter
	{
		NONE = 0,
		LINEAR,
		NEAREST
	};

	enum class TextureFormat
	{
		NONE = 0,
		RED,
		RGB,
		RGBA,
		LUMINANCE,
		LUMINANCE_ALPHA
	};

	struct TextureParameters
	{
		TextureFormat format;
		TextureFilter filter;
		TextureWrap wrap;

		TextureParameters(TextureFormat format = TextureFormat::RGBA, TextureFilter filter = TextureFilter::LINEAR, TextureWrap wrap = TextureWrap::CLAMP)
			: format(format), filter(filter), wrap(wrap)
		{
		}
	};

	class Texture
	{
	public:
		Texture(const String& path, TextureParameters parameters = TextureParameters());
		~Texture();

		void Bind();
		void Unbind();

		void SetData(byte* imageData, uint size);

		inline const int GetWidth() const { return m_Width; }
		inline const int GetHeight() const { return m_Height; }
		
		inline const uint GetTextureID() const { return m_TextureID; }
		inline const String& GetPath() const { return m_Path; }
	private:
		friend class TextureParameters;

		const String& m_Path;
		int m_Width, m_Height;
		TextureParameters m_Parameters;
		uint m_TextureID;

		uint LoadFromFile(const String& path);
		uint Init(byte* imageData, uint width, uint height);

		byte GetStrideFromFormat(TextureFormat format);

		static uint TextureFormatToGL(TextureFormat format);
		static uint TextureWrapToGL(TextureWrap wrap);
		static uint TextureFilterToGL(TextureFilter filter);
	};

}