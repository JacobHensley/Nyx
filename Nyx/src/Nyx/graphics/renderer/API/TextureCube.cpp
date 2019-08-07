#include "NXpch.h"
#include "TextureCube.h"
#include "Glad/glad.h"
#include "stbimage/stb_image.h"

namespace Nyx {

	TextureCube::TextureCube(const String& path)
		: m_Path(path)
	{
		LoadFromFile(path);
	}

	TextureCube::~TextureCube()
	{
	}

	void TextureCube::bind()
	{
		glBindTexture(GL_TEXTURE_2D, m_TextureID);
	}

	void TextureCube::unbind()
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	uint TextureCube::LoadFromFile(const String& path)
	{
		int bpp;
		byte* imageData = stbi_load(path.c_str(), &m_Width, &m_Height, &bpp, STBI_rgb);
		NX_CORE_ASSERT(imageData, "Image Data is Null");

		int faceWidth = m_Width / 4;
		int faceHeight = m_Height / 3;
		NX_CORE_ASSERT(faceWidth == faceHeight, "Image is not the right size");

		std::array<byte*, 6> faceBuffers;

		for (int i = 0; i < faceBuffers.size(); i++)
			faceBuffers[i] = new byte[faceWidth * faceHeight * 3];

		int faceIndex = 0;

		int y = 1;
		for (int x = 0; x < 4; x++)
		{
			int xFaceOffset = x * faceWidth;
			int yFaceOffset = y * faceHeight;

			byte* dest = faceBuffers[faceIndex];
			for (int yPixel = 0; yPixel < faceHeight; yPixel++)
			{
				int yOffset = yFaceOffset + yPixel;
				for (int xPixel = 0; xPixel < faceWidth; xPixel++)
				{
					int xOffset = xFaceOffset + xPixel;
					dest[(xPixel + yPixel * faceWidth) * bpp + 0] = imageData[(xOffset + yOffset * m_Width) * bpp + 0];
					dest[(xPixel + yPixel * faceWidth) * bpp + 1] = imageData[(xOffset + yOffset * m_Width) * bpp + 1];
					dest[(xPixel + yPixel * faceWidth) * bpp + 2] = imageData[(xOffset + yOffset * m_Width) * bpp + 2];
				}
			}
			faceIndex++;
		}

		int x = 1;
		for (int y = 0; y < 3; y++)
		{
			if (y != 1)
			{
				int xFaceOffset = x * faceWidth;
				int yFaceOffset = y * faceHeight;

				NX_CORE_DEBUG("X {0}, Y {1}, Index {2}, ", xFaceOffset, yFaceOffset, faceIndex);

				byte* dest = faceBuffers[faceIndex];
				for (int yPixel = 0; yPixel < faceHeight; yPixel++)
				{
					int yOffset = yFaceOffset + yPixel;
					for (int xPixel = 0; xPixel < faceWidth; xPixel++)
					{
						int xOffset = xFaceOffset + xPixel;
						dest[(xPixel + yPixel * faceWidth) * bpp + 0] = imageData[(xOffset + yOffset * m_Width) * bpp + 0];
						dest[(xPixel + yPixel * faceWidth) * bpp + 1] = imageData[(xOffset + yOffset * m_Width) * bpp + 1];
						dest[(xPixel + yPixel * faceWidth) * bpp + 2] = imageData[(xOffset + yOffset * m_Width) * bpp + 2];
					}
				}
				faceIndex++;
			}
		}


		glGenTextures(1, &m_TextureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureID);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		auto format = GL_RGB;
		auto internalFormat = GL_RGB;
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, internalFormat, faceWidth, faceHeight, 0, format, GL_UNSIGNED_BYTE, faceBuffers[2]);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, internalFormat, faceWidth, faceHeight, 0, format, GL_UNSIGNED_BYTE, faceBuffers[0]);

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, internalFormat, faceWidth, faceHeight, 0, format, GL_UNSIGNED_BYTE, faceBuffers[4]); 
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, internalFormat, faceWidth, faceHeight, 0, format, GL_UNSIGNED_BYTE, faceBuffers[5]);

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, internalFormat, faceWidth, faceHeight, 0, format, GL_UNSIGNED_BYTE, faceBuffers[1]);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, internalFormat, faceWidth, faceHeight, 0, format, GL_UNSIGNED_BYTE, faceBuffers[3]); 

		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

		glBindTexture(GL_TEXTURE_2D, 0);

		for (int i = 0; i < faceBuffers.size(); i++)
			delete[] faceBuffers[i];

		stbi_image_free(imageData);


		NX_CORE_DEBUG("{0}", faceIndex);
		return uint();
	}

}