#include "NXpch.h"
#include "Material.h"
#include "glad/glad.h"

namespace Nyx {
	Material::Material(Shader* shader)
		:	m_Shader(shader)
	{
		m_UniformBuffer = (byte*)malloc(m_Shader->GetUniformSize());
		memset(m_UniformBuffer, 0, m_Shader->GetUniformSize());
	}

	Material::~Material()
	{
	}

	void Material::Bind()
	{
		m_Shader->Bind();
		UploadUniformBuffer();
	}

	void Material::Unbind()
	{
		m_Shader->Unbind();
	}

	void Material::SetTexture(const String& name, Texture* texture)
	{
		ShaderUniform* uniform = m_Shader->FindUniform(name);

		uint slot = uniform->GetSampler();
		if (m_Textures.size() <= slot)
			m_Textures.resize(slot + 1);
		m_Textures[slot] = texture;
	}

	void Material::SetTexture(const String& name, TextureCube* texture)
	{
		ShaderUniform* uniform = m_Shader->FindUniform(name);

		uint slot = uniform->GetSampler();
		if (m_TextureCubes.size() <= slot)
			m_TextureCubes.resize(slot + 1);
		m_TextureCubes[slot] = texture;
	}

	void Material::UploadUniformBuffer()
	{
		for (int i = 0;i < m_Shader->GetUniforms().size();i++)
		{
			ShaderUniform* uniform = m_Shader->GetUniforms()[i];
			const String& name = uniform->GetName();
			int offset = uniform->GetOffset();
			switch (uniform->GetType())
			{
			case Type::SHADER_FLOAT:
				m_Shader->SetUniform1f(name, *(float*)&m_UniformBuffer[offset]);
				break;
			case Type::SHADER_INT:
				m_Shader->SetUniform1i(name, *(int*)&m_UniformBuffer[offset]);
				break;
			case Type::SHADER_VEC2:
				m_Shader->SetUniform2f(name, *(glm::vec2*)&m_UniformBuffer[offset]);
				break;
			case Type::SHADER_VEC3:
				m_Shader->SetUniform3f(name, *(glm::vec3*)&m_UniformBuffer[offset]);
				break;
			case Type::SHADER_VEC4:
				m_Shader->SetUniform4f(name, *(glm::vec4*)&m_UniformBuffer[offset]);
				break;
			case Type::SHADER_MAT4:
				m_Shader->SetUniformMat4(name, *(glm::mat4*)&m_UniformBuffer[offset]);
				break;
			case Type::SHADER_BOOL:
				m_Shader->SetUniformBool(name, *(bool*)&m_UniformBuffer[offset]);
				break;
			case Type::SHADER_SAMPLER2D:
				m_Textures[uniform->GetSampler()]->Bind(uniform->GetSampler());
				break;
			case Type::SHADER_SAMPLERCUBE:
				m_TextureCubes[uniform->GetSampler()]->Bind(uniform->GetSampler());
				break;
			default:
				NX_CORE_ASSERT(false, "Unknown type");
			}
		}
	}

}