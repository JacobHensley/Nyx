#pragma once
#include "Nyx/graphics/renderer/API/Texture.h"
#include "Nyx/graphics/renderer/shaders/Shader.h"

namespace Nyx {

	class Material
	{
	public:
		Material(Shader* shader);
		~Material();

		void Bind();
		void Unbind();

		template<typename T>
		bool SetUniform(const String& name, const T& data)
		{
			ShaderUniform* uniform = m_Shader->FindUniform(name);

			if (uniform == nullptr)
				return false;
			
			switch (uniform->GetType())
			{
				case Type::SHADER_FLOAT:
					m_Shader->SetUniform1f(name, *(float*)&data);
					break;
				case Type::SHADER_INT:
					m_Shader->SetUniform1i(name, *(int*)&data);
					break;
				case Type::SHADER_VEC2:
					m_Shader->SetUniform2f(name, *(glm::vec2*)&data);
					break;
				case Type::SHADER_VEC3:
					m_Shader->SetUniform3f(name, *(glm::vec3*)&data);
					break;
				case Type::SHADER_VEC4:
					m_Shader->SetUniformVec4(name, *(glm::vec4*)&data);
					break;
				case Type::SHADER_MAT4:
					m_Shader->SetUniformMat4(name, *(glm::mat4*)&data);
					break;
				case Type::SHADER_BOOL:
					m_Shader->SetUniformBool(name, *(bool*)&data);
					break;
				case Type::SHADER_SAMPLER2D:
					m_Shader->SetUniform1i(name, uniform->GetSampler());
					(*(Texture*)&data).Bind(uniform->GetSampler());
					break;
				case Type::SHADER_SAMPLERCUBE:
					m_Shader->SetUniform1i(name, uniform->GetSampler());
					(*(TextureCube*)&data).Bind(uniform->GetSampler());
					break;
				default:
					NX_CORE_ASSERT(false, "Unknown type");
			}

			return true;
		}

	private:
		Shader* m_Shader;
	};
}