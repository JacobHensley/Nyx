#pragma once
#include "Nyx/graphics/renderer/API/Texture.h"
#include "Nyx/graphics/renderer/Shader.h"

namespace Nyx {

	class Material
	{
	public:
		Material(Shader* shader);
		~Material();

	public:
		void Bind();
		void Unbind();

		void SetTexture(const String& name, Texture* texture);
		void SetTexture(const String& name, TextureCube* texture);

		template<typename T>
		bool SetUniform(const String& name, const T& data)
		{
			ShaderUniform* uniform = m_Shader->FindUniform(name);

			if (uniform == nullptr) 
			{
				return false;
			}

			memcpy(m_UniformBuffer + uniform->GetOffset(), &data, uniform->GetSize());
			return true;
		}

	private:
		void UploadUniformBuffer();

		Shader* m_Shader;
		byte* m_UniformBuffer;

		std::vector<Texture*> m_Textures;
		std::vector<TextureCube*> m_TextureCubes;
	};
}