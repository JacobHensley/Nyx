#pragma once
#include "Nyx/Core/Core.h"
#include "Nyx/Asset/Asset.h"
#include "Nyx/Graphics/ShaderUniform.h"

#include <glm/glm.hpp>
#include <shaderc/shaderc.hpp>
#include <spirv_cross.hpp>

#include <String>
#include <unordered_map>

typedef unsigned int GLenum;

namespace Nyx {

	enum class ShaderType
	{
		NONE = -1, VERTEX, FRAGMENT, COMPUTE, GEOMETRY
	};

	enum class UniformSystemType
	{
		NONE = -1, MATERIAL, RENDERER
	};

	struct UniformBuffer
	{
		std::string name;
		uint index;
		uint bindingPoint;
		uint size;
		uint openGLID;
		std::vector<Ref<ShaderUniform>> uniforms;
	};

	class Shader : public Asset
	{
	public:
		Shader(const std::string& path);
		
		void Bind();
		void Unbind();

		void Reload();

		void UploadUniformBuffer(uint index, byte* buffer, uint size);

		std::vector<UniformBuffer*> GetUniformBuffers(UniformSystemType type);
		const std::vector<Ref<ShaderResource>>& GetResources(UniformSystemType type);

		Ref<ShaderResource> FindShaderResource(const std::string& name, UniformSystemType type);

		inline const std::string& GetPath() const { return m_Path; }

		void SetUniform1i(const std::string& name, int value);
		void SetUniform1f(const std::string& name, float value);
		void SetUniform2f(const std::string& name, const glm::vec2& vec);
		void SetUniform3f(const std::string& name, const glm::vec3& vec);
		void SetUniform4f(const std::string& name, const glm::vec4& vec);
		void SetUniformMat4(const std::string& name, const glm::mat4& matrix);

		uint64_t GetHash() const
		{
			return std::hash<std::string>{}(m_Path);
		}
	private:
		void Init();
		std::unordered_map<ShaderType, std::string> SplitShaders(const std::string& path);
		uint CompileShaders(const std::unordered_map<ShaderType, std::string>& shaderSrc);
		void SpirvReflect(std::vector<uint32_t>& data);

		RendererID GetRendererUniformID(const std::string& name);

		shaderc_shader_kind ShaderTypeToShaderc(ShaderType type);
		GLenum ShaderTypeToGL(ShaderType type);
		const std::string& ShaderTypeToString(ShaderType type);

	private:
		std::string m_Path;
		uint m_ShaderID;

		std::unordered_map<ShaderType, std::string> m_ShaderSrc;
		std::vector<UniformBuffer> m_UniformBuffers;
		std::unordered_map<UniformSystemType, std::vector<Ref<ShaderResource>>> m_Resources;
	};

}