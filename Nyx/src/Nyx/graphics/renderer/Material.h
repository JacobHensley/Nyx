#pragma once
#include "Nyx/graphics/renderer/API/Texture.h"
#include "Nyx/graphics/renderer/Shader.h"

namespace Nyx {

	class Material
	{
	public:
		Material(Ref<Shader> shader);
		~Material();

	public:
		void Bind();
		void Unbind();

		void SetTexture(const String& name, Ref<Texture> texture);
		void SetTexture(const String& name, Ref<TextureCube> texture);

		inline bool GetDepthTesting() { return m_DepthTesting; }
		inline void SetDepthTesting(bool depthTesting) { m_DepthTesting = depthTesting; }

		inline Ref<Shader> GetShader() { return m_Shader; }

		template<typename T>
		bool SetUniform(const String& name, const T& data)
		{
			Ref<ShaderUniform> uniform = m_Shader->FindUserUniform(name);

			if (uniform == nullptr) 
			{
				return false;
			}

			memcpy(m_UniformBuffer + uniform->GetOffset(), &data, uniform->GetSize());
			return true;
		}

	private:
		void UploadUniformBuffer();

		bool m_DepthTesting = true;

		Ref<Shader> m_Shader;
		byte* m_UniformBuffer;

		std::vector<Ref<Texture>> m_Textures;
		std::vector<Ref<TextureCube>> m_TextureCubes;
	};
}