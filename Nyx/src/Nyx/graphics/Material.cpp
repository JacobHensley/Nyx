#include "NXpch.h"
#include "Material.h"

namespace Nyx
{
	Material::Material(Ref<Shader> shader)
		: m_Shader(shader)
	{
		m_UniformDataBuffer = (byte*)malloc(m_Shader->GetMaterialUniformBufferSize());
		memset(m_UniformDataBuffer, 0, m_Shader->GetMaterialUniformBufferSize());

		m_Textures.resize(32);
		m_TextureCubes.resize(32);
	}

	void Material::Bind()
	{
		m_Shader->Bind();
		BindTextures();
		UploadUniforms();
	}

	void Material::Unbind()
	{
		m_Shader->Unbind();
	}

	bool Material::Reload()
	{
		return false;
	}

	void Material::SetTexture(const std::string& name, Ref<Texture>& texture)
	{
		const std::unordered_map<std::string, ShaderResource>& uniforms = m_Shader->GetResources();
		
		if (uniforms.find(name) != uniforms.end())
		{
			ShaderResource resource = m_Shader->GetResources().at(name);
			NX_CORE_ASSERT(resource.Type == UniformType::TEXTURE_2D, "Resource is not type of TEXTURE_2D");

			uint slot = resource.TextureUnit;
			if (m_Textures.size() <= slot)
			{
				m_Textures.resize(slot + 1);
			}

			m_Textures[slot] = texture;
			return;
		}

		NX_CORE_WARN("Texture {0} is not active or does not exist in shader {1}", name, m_Shader->GetPath());
	}

	void Material::SetTexture(const std::string& name, Ref<TextureCube>& texture)
	{
		const std::unordered_map<std::string, ShaderResource>& uniforms = m_Shader->GetResources();

		if (uniforms.find(name) != uniforms.end())
		{
			ShaderResource resource = m_Shader->GetResources().at(name);
			NX_CORE_ASSERT(resource.Type == UniformType::TEXTURE_CUBE, "Resource is not type of TEXTURE_CUBE");

			uint slot = resource.TextureUnit;
			if (m_TextureCubes.size() <= slot)
			{
				m_TextureCubes.resize(slot + 1);
			}

			m_TextureCubes[slot] = texture;
			return;
		}

		NX_CORE_WARN("Texture Cube {0} is not active or does not exist in shader {1}", name, m_Shader->GetPath());
	}

	void Material::BindTextures()
	{
		for (int i = 0; i < m_Textures.size(); i++)
		{
			if (m_Textures[i] != nullptr)
			{
				m_Textures[i]->Bind(i);
			}
		}

		for (int i = 0; i < m_TextureCubes.size(); i++)
		{
			if (m_TextureCubes[i] != nullptr)
			{
				m_TextureCubes[i]->Bind(i);
			}				
		}
	}

	void Material::UploadUniforms()
	{
		const std::unordered_map<std::string, ShaderUniform>& uniforms = m_Shader->GetMaterialUniforms();

		for (auto const& [name, uniform] : uniforms)
		{
			int offset = uniform.Offset;
			switch (uniform.Type)
			{
				case UniformType::FLOAT:
					m_Shader->SetUniformFloat(name, *(float*)&m_UniformDataBuffer[offset]);
					break;
				case UniformType::INT:
					m_Shader->SetUniformInt(name, *(int*)&m_UniformDataBuffer[offset]);
					break;
				case UniformType::FLOAT2:
					m_Shader->SetUniformFloat2(name, *(glm::vec2*)&m_UniformDataBuffer[offset]);
					break;
				case UniformType::FLOAT3:
					m_Shader->SetUniformFloat3(name, *(glm::vec3*)&m_UniformDataBuffer[offset]);
					break;
				case UniformType::FLOAT4:
					m_Shader->SetUniformFloat4(name, *(glm::vec4*)&m_UniformDataBuffer[offset]);
					break;
				case UniformType::MAT4:
					m_Shader->SetUniformMat4(name, *(glm::mat4*)&m_UniformDataBuffer[offset]);
					break;
				case UniformType::BOOL:
					m_Shader->SetUniformInt(name, *(bool*)&m_UniformDataBuffer[offset]);
					break;
				default:
					NX_CORE_ASSERT(false, "Unknown type");
			}
		}
	}

}
