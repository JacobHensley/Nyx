#include "NXpch.h"
#include "Material.h"
#include "glad/glad.h"

namespace Nyx {

	Material::Material(Ref<Shader> shader)
		:	m_Shader(shader)
	{
		std::vector<UniformBuffer*> uniformBuffers = m_Shader->GetUniformBuffers(UniformSystemType::MATERIAL);
		for (UniformBuffer* uniformBuffer : uniformBuffers)
		{
			MaterialBuffer& materialBuffer = m_MaterialBuffers[uniformBuffer->index];

			materialBuffer.size = uniformBuffer->size;
			materialBuffer.data = new byte[uniformBuffer->size];
			memset(materialBuffer.data, 0, uniformBuffer->size);
			materialBuffer.index = uniformBuffer->index;

			for (Ref<ShaderUniform> uniform : uniformBuffer->uniforms)
			{
				m_Uniforms[uniform->GetName()] = { uniformBuffer->index, uniform };
			}
		}

	}

	Material::~Material()
	{
	}

	void Material::Bind()
	{
		m_Shader->Bind();

		BindTextures();

		UploadUniformBuffers();
	}

	void Material::Unbind()
	{
		m_Shader->Unbind();
	}

	void Material::BindTextures()
	{
		for (int i = 0; i < m_Textures.size(); i++)
		{
			if (m_Textures[i] != nullptr)
				m_Textures[i]->Bind(i);
		}

		for (int i = 0; i < m_TextureCubes.size(); i++)
		{
			if (m_TextureCubes[i] != nullptr)
				m_TextureCubes[i]->Bind(i);
		}
	}

	void Material::UploadUniformBuffers()
	{
		for (auto& kv : m_MaterialBuffers)
		{
			MaterialBuffer & materialBuffer = kv.second;
			m_Shader->UploadUniformBuffer(materialBuffer.index, materialBuffer.data, materialBuffer.size);
		}
	}

	void Material::SetTexture(const String& name, Ref<Texture>& texture)
	{
		Ref<ShaderResource> resource = m_Shader->FindShaderResource(name, UniformSystemType::MATERIAL);

		if (resource == nullptr)
			return;

		uint slot = resource->GetSampler();
		if (m_Textures.size() <= slot)
		{
			m_Textures.resize(slot + 1);
		}

		m_Textures[slot] = texture;
	}

	void Material::SetTexture(const String& name, Ref<TextureCube>& texture)
	{
		Ref<ShaderResource> resource = m_Shader->FindShaderResource(name, UniformSystemType::MATERIAL);

		if (resource == nullptr)
			return;

		uint slot = resource->GetSampler();
		if (m_TextureCubes.size() <= slot)
		{
			m_TextureCubes.resize(slot + 1);
		}

		m_TextureCubes[slot] = texture;
	}

}