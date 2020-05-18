#pragma once
#include "Nyx/graphics/renderer/API/Texture.h"
#include "Nyx/graphics/renderer/Shader.h"

namespace Nyx {

	struct MaterialBuffer
	{
		byte* data;
		uint size;
		uint32_t index;
	};

	class Material
	{
	public:
		Material(Ref<Shader> shader);
		~Material();

	public:
		void Bind();
		void Unbind();

		void SetTexture(const String& name, Ref<Texture>& texture);
		void SetTexture(const String& name, Ref<TextureCube>& texture);

		inline Ref<Shader> GetShader() { return m_Shader; }

		inline void SetDepthTesting(bool value) { m_DepthTesting = value; }
		inline bool GetDepthTesting() { return m_DepthTesting; }

		template<typename T>
		void Set(const String& name, const T& data)
		{
			auto[bufferIndex, uniform] = m_Uniforms[name];
			if (uniform == nullptr)
				return;

			memcpy(m_MaterialBuffers[bufferIndex - 1].data + uniform->GetOffset(), &data, uniform->GetSize());
		}

	private:
		Ref<Shader> m_Shader;
		std::vector<MaterialBuffer> m_MaterialBuffers;
		std::unordered_map<String, std::pair<uint, Ref<ShaderUniform>>> m_Uniforms;

		std::vector<Ref<Texture>> m_Textures;
		std::vector<Ref<TextureCube>> m_TextureCubes;

		bool m_DepthTesting = true;
	};
}