#pragma once
#include "Nyx/Core/Core.h"
#include "Nyx/Asset/Asset.h"

#include <glm/glm.hpp>
#include <shaderc/shaderc.hpp>

#include <String>
#include <unordered_map>

typedef unsigned int GLenum;

namespace Nyx {

	enum class ShaderStage
	{
		NONE = -1, VERTEX, FRAGMENT, COMPUTE
	};

	enum class UniformType
	{
		NONE = -1, BOOL, INT, FLOAT, FLOAT2, FLOAT3, FLOAT4, MAT4, TEXTURE_2D, TEXTURE_CUBE, IMAGE_2D, IMAGE_CUBE
	};

	struct ShaderUniform
	{
		ShaderUniform() = default;
		ShaderUniform(const std::string name, UniformType type, uint32_t size, uint32_t offset)
			: Name(name), Type(type), Size(size), Offset(offset)
		{
		}

		std::string Name;
		UniformType Type;
		uint32_t Size;
		uint32_t Offset;
	};

	struct ShaderResource
	{
		ShaderResource() = default;
		ShaderResource(const std::string name, UniformType type, uint32_t textureUnit)
			: Name(name), Type(type), TextureUnit(textureUnit)
		{
		}

		std::string Name;
		UniformType Type;
		uint32_t TextureUnit;
	};

	class Shader : public Asset
	{
	public:
		Shader(const std::string& path);

		void Bind();
		void Unbind();
		bool Reload();

		inline const std::string& GetPath() const { return m_Path; }
		inline uint32_t GetShaderProgram() const { return m_ShaderProgram; }

		inline const std::unordered_map<std::string, ShaderUniform>& GetRendererUniforms() const { return m_RendererUniforms; }
		inline const std::unordered_map<std::string, ShaderUniform>& GetMaterialUniforms() const { return m_MaterialUniforms; }
		inline const std::unordered_map<std::string, ShaderResource>& GetResources() const { return m_Resources; }

		inline uint32_t GetMaterialUniformBufferSize() { return m_MaterialUniformBufferSize; }
		static uint32_t GetUniformSizeFromType(UniformType type);

		uint32_t SetUniformInt(const std::string& name, int value);
		void SetUniformIntArray(const std::string& name, int* values, uint32_t count);
		void SetUniformFloat(const std::string& name, float value);
		void SetUniformFloat2(const std::string& name, const glm::vec2& value);
		void SetUniformFloat3(const std::string& name, const glm::vec3& value);
		void SetUniformFloat4(const std::string& name, const glm::vec4& value);
		void SetUniformMat4(const std::string& name, const glm::mat4& value);

		uint64_t GetHash() const
		{
			return std::hash<std::string>{}(m_Path);
		}

	private:
		bool Init();

		std::unordered_map<ShaderStage, std::string> SplitShaders(const std::string& path);
		bool CompileShaders(const std::unordered_map<ShaderStage, std::string>& shaderSrc);
		void ShaderProgramReflect();

	private:
		std::string m_Path;
		uint32_t m_ShaderProgram;
		bool m_Initialized = false;

		std::unordered_map<ShaderStage, std::string> m_ShaderSrc;

		std::unordered_map<std::string, ShaderUniform> m_RendererUniforms;
		std::unordered_map<std::string, ShaderUniform> m_MaterialUniforms;
		std::unordered_map<std::string,ShaderResource> m_Resources;

		uint32_t m_MaterialUniformBufferSize = 0;
	};

}