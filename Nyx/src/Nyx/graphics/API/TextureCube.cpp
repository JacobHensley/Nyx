#include "NXpch.h"
#include "TextureCube.h"
#include "Nyx/graphics/Shader.h"
#include "Nyx/graphics/SceneRenderer.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

#include <Glad/glad.h>
#include <stbimage/stb_image.h>

namespace Nyx {

	TextureCube::TextureCube(const std::string& path)
		: m_Path(path)
	{
		LoadFromFile(path);
		NX_CORE_INFO("Created TextureCube at Path: {0}", path);
	}

	TextureCube::TextureCube(uint32_t id, uint32_t width, uint32_t height)
		:	m_TextureID(id), m_Width(width), m_Height(height)
	{
	}

	TextureCube::~TextureCube()
	{
	}

	void TextureCube::Bind(uint32_t slot)
	{
		glBindTextureUnit(slot, m_TextureID);
	}

	void TextureCube::Unbind()
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

	Ref<TextureCube> TextureCube::CalculateIrradianceMap()
	{
		uint32_t mapSize = 32;

		Ref<Shader> irradianceMapShader = SceneRenderer::GetComputeIrradianceMapShader();
		
		uint32_t irradianceMapID;
		glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &irradianceMapID);
		glTextureStorage2D(irradianceMapID, 1, GL_RGBA16F, mapSize, mapSize);
		glTextureParameteri(irradianceMapID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(irradianceMapID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		irradianceMapShader->Bind();
		glBindTextureUnit(0, m_TextureID);
		glBindImageTexture(0, irradianceMapID, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
		glDispatchCompute(mapSize / 32, mapSize / 32, 6);

		return CreateRef<TextureCube>(irradianceMapID, mapSize, mapSize);
	}

	void TextureCube::LoadFromFile(const std::string& path)
	{
		uint32_t mapSize = 1024;
		uint32_t numMips = 11;

		m_Width = 1024;
		m_Height = 1024;

		// Create empty cubemap
		uint32_t unfilteredCubemapID;
		glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &unfilteredCubemapID);
		glTextureStorage2D(unfilteredCubemapID, numMips, GL_RGBA16F, mapSize, mapSize);
		glTextureParameteri(unfilteredCubemapID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(unfilteredCubemapID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Load texture from disk
		Texture equirectTexture = Texture(path, TextureParameters(TextureFormat::RGBA16F));

		// Map texture from disk into cubemap
		Ref<Shader> equirectToCubemapShader = SceneRenderer::GetEquirectToCubemapShader();
		equirectToCubemapShader->Bind();
		glBindTextureUnit(0, equirectTexture.GetTextureID());
		glBindImageTexture(0, unfilteredCubemapID, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
		glDispatchCompute(mapSize / 32, mapSize / 32, 6);
		
		glGenerateTextureMipmap(unfilteredCubemapID);

		// Create cubemap to hold filtered map
		glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_TextureID);
		glTextureStorage2D(m_TextureID, numMips, GL_RGBA16F, mapSize, mapSize);
		glTextureParameteri(m_TextureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_TextureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Copy cubemap into empty cubemap
		glCopyImageSubData(unfilteredCubemapID, GL_TEXTURE_CUBE_MAP, 0, 0, 0, 0, m_TextureID, GL_TEXTURE_CUBE_MAP, 0, 0, 0, 0, mapSize, mapSize, 6);

		// Filter cubemap
		Ref<Shader> filterCubemapShader = SceneRenderer::GetFilterCubemapShader();

		filterCubemapShader->Bind();
		glBindTextureUnit(0, unfilteredCubemapID);

		const float deltaRoughness = 1.0f / numMips;
		for (int level = 1, size = mapSize / 2; level <= numMips; ++level, size /= 2) 
		{
			const GLuint numGroups = glm::max(1, size / 32);
			glBindImageTexture(0, m_TextureID, level, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
			glProgramUniform1f(filterCubemapShader->GetShaderProgram(), 0, level * deltaRoughness);
			glDispatchCompute(numGroups, numGroups, 6);
		}

		glDeleteTextures(1, &unfilteredCubemapID); 
	}

}