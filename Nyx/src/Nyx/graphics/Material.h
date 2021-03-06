#pragma once
#include "Nyx/Core/Core.h"
#include "Nyx/Asset/Asset.h"
#include "Nyx/Graphics/API/TextureCube.h"
#include "Nyx/Graphics/API/Texture.h"
#include "Nyx/Graphics/Shader.h"
#include <String>

namespace Nyx {

	struct MaterialBuffer
	{
		byte* data;
		uint size;
		uint32_t index;
	};

	class Material : public Asset
	{
	public:
		Material(Ref<Shader> shader, uint32_t materialSortKey = 0, bool opaque = true);
		~Material();

	public:
		void Bind();
		void Unbind();

		void BindTextures();
		void UploadUniformBuffers();

		void SetTexture(const std::string& name, Ref<Texture>& texture);
		void SetTexture(const std::string& name, Ref<TextureCube>& texture);

		inline Ref<Shader> GetShader() { return m_Shader; }

		inline void SetDepthTesting(bool value) { m_DepthTesting = value; }
		inline bool GetDepthTesting() { return m_DepthTesting; }

		uint32_t GetMaterialSortKey() const { return m_MaterialSortKey; }

		template<typename T>
		void Set(const std::string& name, const T& data)
		{
			auto[bufferIndex, uniform] = m_Uniforms[name];
			if (uniform == nullptr)
				return;

			memcpy(m_MaterialBuffers[bufferIndex].data + uniform->GetOffset(), &data, uniform->GetSize());
		}

		template<>
		void Set(const std::string& name, const bool& data)
		{
			auto [bufferIndex, uniform] = m_Uniforms[name];
			if (uniform == nullptr)
				return;

			int intData = (int)data;
			memcpy(m_MaterialBuffers[bufferIndex].data + uniform->GetOffset(), &intData, uniform->GetSize());
		}

		uint64_t GetHash() const
		{
			m_Shader->GetHash() + m_MaterialSortKey;
		}

		bool operator==(const Material& other) const
		{
			return GetHash() == other.GetHash();
		}

		bool operator!=(const Material& other) const
		{
			return !(*this == other);
		}

		bool operator<(const Material& other) const
		{
			if (m_Opaque == other.m_Opaque)
				return m_MaterialSortKey < other.m_MaterialSortKey;

			return m_Opaque;
		}

		bool IsOpaque() const { return m_Opaque; }
	private:
		Ref<Shader> m_Shader;
		std::unordered_map<uint, MaterialBuffer> m_MaterialBuffers;
		std::unordered_map<std::string, std::pair<uint, Ref<ShaderUniform>>> m_Uniforms;

		std::vector<Ref<Texture>> m_Textures;
		std::vector<Ref<TextureCube>> m_TextureCubes;

		bool m_DepthTesting = true;
		bool m_Opaque = true;
		uint32_t m_MaterialSortKey = 0;
	};
}
