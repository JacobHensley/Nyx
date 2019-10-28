#include "NXpch.h"
#include "Texture.h"
#include "glad/glad.h"
#include "stbimage/stb_image.h"

namespace Nyx {

	Texture::Texture(const String& path, TextureParameters parameters /*= TextureParameters()*/)
		:	m_Path(path), m_Parameters(parameters)
	{
		m_TextureID = LoadFromFile(path);
		NX_CORE_ASSERT(m_TextureID != -1, "Image Data is Null");
	}

	Texture::Texture(int width, int height, TextureParameters parameters /*= TextureParameters()*/)
		:	m_Path(""), m_Width(width), m_Height(height), m_Parameters(parameters)
	{
		m_TextureID = Init(0, width, height);
	}

	Texture::~Texture()
	{
		glDeleteTextures(1, &m_TextureID);
	}

	void Texture::Bind(uint slot)
	{
		glBindTextureUnit(slot, m_TextureID);
	}

	void Texture::Bind()
	{
		glBindTexture(GL_TEXTURE_2D, m_TextureID);
	}

	void Texture::Unbind()
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	bool Texture::Reload(const String& path)
	{
		uint reloadedTextureID = LoadFromFile(path);
		if (reloadedTextureID == -1) 
		{
			NX_CORE_WARN("Failed to reload texture at path {0}", path);
			return false;
		}

		m_TextureID = reloadedTextureID;
		m_Path = path;
		return true;
	}

	static byte* buffer;
	static byte* AlignData(byte* data, uint size)
	{
		buffer = new byte[size * 4];
		memset(buffer, 0, size * 4);
		for (uint i = 0; i < size; i++) {
			buffer[i * 4] = data[i];
		}

		return buffer;
	}

	void Texture::SetData(byte* imageData, uint size)
	{
		Bind();
		NX_CORE_ASSERT(size == m_Width * m_Height * GetStrideFromFormat(m_Parameters.format), "Size is incorrect");
		byte* alignedData = AlignData(imageData, size);
		delete buffer;
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_Width, m_Height, TextureFormatToGL(m_Parameters.format), GL_UNSIGNED_BYTE, imageData);
		Unbind();
	}

	uint Texture::LoadFromFile(const String& path)
	{
		int bpp;
		byte* imageData = stbi_load(path.c_str(), &m_Width, &m_Height, &bpp, 0);
		if (!imageData)
			return -1;

		uint TextureID = Init(imageData, m_Width, m_Height);

		stbi_image_free(imageData);
		return TextureID;
	}

	uint Texture::Init(byte* imageData, uint width, uint height)
	{
		uint textureID;

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);

		if (m_Parameters.format == TextureFormat::RGBA16F || m_Parameters.format == TextureFormat::RGBA32F)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
		else
			glTexImage2D(GL_TEXTURE_2D, 0, TextureFormatToGL(m_Parameters.format), width, height, 0, TextureFormatToGL(m_Parameters.format), GL_UNSIGNED_BYTE, imageData);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, TextureFilterToGL(m_Parameters.filter));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, TextureFilterToGL(m_Parameters.filter));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, TextureWrapToGL(m_Parameters.wrap));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, TextureWrapToGL(m_Parameters.wrap));

		glBindTexture(GL_TEXTURE_2D, 0);

		return textureID;
	}

	byte Texture::GetStrideFromFormat(TextureFormat format)
	{
		switch (format)
		{
			case TextureFormat::RED:				return 1;
			case TextureFormat::RGB:				return 3;
			case TextureFormat::RGBA:				return 4;
			case TextureFormat::LUMINANCE_ALPHA:	return 4;
		}
		return 0;
	}

	uint Texture::TextureFormatToGL(TextureFormat format)
	{
		switch (format)
		{
			case TextureFormat::RGBA:				return GL_RGBA;
			case TextureFormat::RGBA16F:			return GL_RGBA16F;
			case TextureFormat::RGBA32F:			return GL_RGBA32F;
			case TextureFormat::RGB:				return GL_RGB;
			case TextureFormat::RED:				return GL_RED;
			case TextureFormat::LUMINANCE:			return GL_LUMINANCE;
			case TextureFormat::LUMINANCE_ALPHA:	return GL_LUMINANCE_ALPHA;
		}
		return 0;
	}

	uint Texture::TextureWrapToGL(TextureWrap wrap)
	{
		switch (wrap)
		{
			case TextureWrap::CLAMP:			return GL_CLAMP;
			case TextureWrap::CLAMP_TO_BORDER:	return GL_CLAMP_TO_BORDER;
			case TextureWrap::CLAMP_TO_EDGE:	return GL_CLAMP_TO_EDGE;
			case TextureWrap::REPEAT:			return GL_REPEAT;
			case TextureWrap::MIRRORED_REPEAT:	return GL_MIRRORED_REPEAT;
		}
		return 0;
	}

	uint Texture::TextureFilterToGL(TextureFilter filter)
	{
		switch (filter)
		{
			case TextureFilter::LINEAR:			return GL_LINEAR;
			case TextureFilter::NEAREST:		return GL_NEAREST;
		}
		return 0;
	}

}