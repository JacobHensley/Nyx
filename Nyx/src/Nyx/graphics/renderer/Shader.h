#pragma once
#include "Nyx.h"
#include "NXpch.h"

#include "ShaderUniform.h"
#include <glm/glm.hpp>

#include <shaderc/shaderc.hpp>
#include "spirv_cross.hpp"

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
		String name;
		uint index;
		uint bindingPoint;
		uint size;
		uint openGLID;
		std::vector<Ref<ShaderUniform>> uniforms;
	};

	class Shader
	{
	public:
		Shader(const String& path);
		
		void Bind();
		void Unbind();

		void Reload();

		void UploadUniformBuffer(uint index, byte* buffer, uint size);

		std::vector<UniformBuffer*> GetUniformBuffers(UniformSystemType type);
		const std::vector<Ref<ShaderResource>>& GetResources(UniformSystemType type);

		Ref<ShaderResource> FindShaderResource(const String& name, UniformSystemType type);

		inline const String& GetPath() const { return m_Path; }

		void SetUniform1i(const String& name, int value);
		void SetUniform1f(const String& name, float value);
		void SetUniform2f(const String& name, const glm::vec2& vec);
		void SetUniform3f(const String& name, const glm::vec3& vec);
		void SetUniform4f(const String& name, const glm::vec4& vec);
		void SetUniformMat4(const String& name, const glm::mat4& matrix);

	private:
		void Init();
		std::unordered_map<ShaderType, String> SplitShaders(const String& path);
		uint CompileShaders(std::unordered_map<ShaderType, String> shaderSrc);
		void SpirvReflect(std::vector<uint32_t>& data);

		RendererID GetRendererUniformID(const String& name);

		shaderc_shader_kind ShaderTypeToShaderc(ShaderType type);
		GLenum ShaderTypeToGL(ShaderType type);
		const String& ShaderTypeToString(ShaderType type);

	private:
		String m_Path;
		uint m_ShaderID;

		std::unordered_map<ShaderType, String> m_ShaderSrc;
		std::vector<UniformBuffer> m_UniformBuffers;
		std::unordered_map<UniformSystemType, std::vector<Ref<ShaderResource>>> m_Resources;
	};

}