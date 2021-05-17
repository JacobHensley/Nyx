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
        Material(const std::string& name, Ref<Shader> shader);
        Material(Ref<Shader> shader);

        void Bind();
        void Unbind();
        bool Reload(); // This function is not implemented as of now

        template<typename T>
        T& Get(const std::string& name)
        {
            const std::unordered_map<std::string, ShaderUniform>& uniforms = m_Shader->GetMaterialUniforms();
            if (uniforms.find(name) != uniforms.end())
            {
                ShaderUniform uniform = uniforms.at(name);
                return *(T*)&m_UniformDataBuffer[uniform.Offset];
            }

            NX_CORE_WARN("Could not find uniform {0} in buffer", name);
        }

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

        inline const std::vector<Ref<Texture>>& GetTextures() { return m_Textures; }

        inline Ref<Shader> GetShader() const { return m_Shader; }
        inline const std::string& GetName() const { return m_Name; }

        inline int GetMaterialFlags() const { return m_MaterialFlags; }
        void SetMaterialFlags(int flags) { m_MaterialFlags = flags; }
        void SetMaterialFlag(MaterialFlags flag) { m_MaterialFlags |= (int)flag; }

        uint32_t GetMaterialSortKey() const { return m_MaterialSortKey; }
        void SetMaterialSortKey(uint32_t sortKey) { m_MaterialSortKey = sortKey; }

        bool IsOpaque() const { return m_IsOpaque; }
        void SetOpaque(bool opaque) { m_IsOpaque = opaque; }

        uint64_t GetHash() const
        {
            return m_Shader->GetHash() + m_MaterialSortKey;
        }

        uint32_t HashBuffer() const
        {
         /*   uint32_t result = 0;
            const uint32_t prime = 31;
            for (uint32_t i = 0; i < m_Shader->GetMaterialUniformBufferSize(); ++i) {
                result = m_UniformDataBuffer[i] + (result * prime);
            }
            return result; */
            
            // TEST
            uint32_t texturesHash = 0;
            for (int i = 0; i < m_Textures.size(); i++)
            {
                if (m_Textures[i] != nullptr)
                    texturesHash += std::hash<std::string>{}(m_Textures[i]->GetPath());
            }
            return std::hash<std::string_view>()(std::string_view((char*)m_UniformDataBuffer, m_Shader->GetMaterialUniformBufferSize())) + texturesHash;
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
        /*    if (m_IsOpaque == other.m_IsOpaque)
                return m_MaterialSortKey < other.m_MaterialSortKey;

            return m_IsOpaque; */

            return HashBuffer() < other.HashBuffer();
        }

    private:
        void Init();
        void BindTextures();
        void UploadUniforms();

    private:
        Ref<Shader> m_Shader;
        const std::string m_Name;

        int m_MaterialFlags = 0;
        uint32_t m_MaterialSortKey = 0;
        bool m_IsOpaque = false;

        byte* m_UniformDataBuffer;
       
        std::vector<Ref<Texture>> m_Textures;
        std::vector<Ref<TextureCube>> m_TextureCubes;
    };
}