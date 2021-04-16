#pragma once
#include "Nyx/Core/Core.h"
#include "Nyx/Asset/Asset.h"
#include "Nyx/Graphics/API/TextureCube.h"
#include "Nyx/Graphics/API/Texture.h"
#include "Nyx/Graphics/Shader.h"
#include <String>

namespace Nyx {

    enum class MaterialFlags
    {
        NONE = 0,                      
        DISABLE_DEPTH_TEST    = BIT(0),
        WIREFRAME             = BIT(1),
        DOUBLE_SIDED          = BIT(2),
        BACK_TO_FRONT         = BIT(3)
    };

    class Material : public Asset
    {
    public:
        Material(Ref<Shader> shader);

        void Bind();
        void Unbind();
        bool Reload(); // This function is not implemented as of now

        template<typename T>
        void Set(const std::string& name, const T& data)
        {
            const std::unordered_map<std::string, ShaderUniform>& uniforms = m_Shader->GetMaterialUniforms();
            if (uniforms.find(name) != uniforms.end())
            {
                ShaderUniform uniform = uniforms.at(name);
                memcpy(m_UniformDataBuffer + uniform.Offset, &data, uniform.Size);
                return;
            }

            NX_CORE_WARN("Uniform {0} is not active in shader {1}", name, m_Shader->GetPath());
        }

        template<>
        void Set(const std::string& name, const bool& data)
        {
            const std::unordered_map<std::string, ShaderUniform>& uniforms = m_Shader->GetMaterialUniforms();
            if (uniforms.find(name) != uniforms.end())
            {
                ShaderUniform uniform = uniforms.at(name);
                int intData = (int)data;
                memcpy(m_UniformDataBuffer + uniform.Offset, &data, uniform.Size);
                return;
            }

            NX_CORE_WARN("Uniform {0} is not active in shader {1}", name, m_Shader->GetPath());
        }

        void SetTexture(const std::string& name, Ref<Texture>& texture);
        void SetTexture(const std::string& name, Ref<TextureCube>& texture);

        inline Ref<Shader> GetShader() const { return m_Shader; }

        inline int GetMaterialFlags() const { return m_MaterialFlags; }
        void SetMaterialFlags(int flags) { m_MaterialFlags = flags; }
        void SetMaterialFlag(MaterialFlags flag) { m_MaterialFlags |= (int)flag; }

        uint32_t GetMaterialSortKey() const { return m_MaterialSortKey; }
        void SetMaterialSortKey(uint32_t sortKey) { m_MaterialSortKey = sortKey; }

        bool IsOpaque() const { return m_IsOpaque; }
        void SetOpaque(bool opaque) { m_IsOpaque = opaque; }

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
            if (m_IsOpaque == other.m_IsOpaque)
                return m_MaterialSortKey < other.m_MaterialSortKey;

            return m_IsOpaque;
        }

    private:
        void BindTextures();
        void UploadUniforms();

    private:
        Ref<Shader> m_Shader;

        int m_MaterialFlags = 0;
        uint32_t m_MaterialSortKey = 0;
        bool m_IsOpaque = false;

        byte* m_UniformDataBuffer;
        std::vector<Ref<Texture>> m_Textures;
        std::vector<Ref<TextureCube>> m_TextureCubes;
    };
}