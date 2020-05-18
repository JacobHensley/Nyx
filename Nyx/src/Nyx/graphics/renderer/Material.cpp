#include "NXpch.h"
#include "Material.h"
#include "glad/glad.h"

namespace Nyx {

	Material::Material(Ref<Shader> shader)
		:	m_Shader(shader)
	{
		std::vector<UniformBuffer*> uniformBuffers = m_Shader->GetUniformBuffers(UniformSystemType::MATERIAL);
		for each (UniformBuffer* uniformBuffer in uniformBuffers)
		{
			MaterialBuffer& materialBuffer = m_MaterialBuffers.emplace_back();

			materialBuffer.size = uniformBuffer->size;
			materialBuffer.data = new byte[uniformBuffer->size];
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

		for (MaterialBuffer& materialBuffer : m_MaterialBuffers)
		{
			m_Shader->UploadUniformBuffer(materialBuffer.index, materialBuffer.data, materialBuffer.size);
		}
	}

	void Material::Unbind()
	{
		m_Shader->Unbind();
	}

	void Material::SetTexture(const String& name, Ref<Texture>& texture)
	{
		Ref<ShaderResource> resource = m_Shader->FindShaderResource(name, UniformSystemType::MATERIAL);

		if (resource == nullptr)
			return;

		uint slot = resource->GetLocation();
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

		uint slot = resource->GetLocation();
		if (m_TextureCubes.size() <= slot)
		{
			m_TextureCubes.resize(slot + 1);
		}

		m_TextureCubes[slot] = texture;
	}

}