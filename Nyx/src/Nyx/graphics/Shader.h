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
		NONE = -1, VERTEX, FRAGMENT
	};

	enum class RendererUniformID
	{
		NONE = -1, TRANSFORM, VIEW_MATRIX, PROJ_MATRIX, INVERSE_VP, MVP, CAMERA_POSITION, BRDF_LUT, IRRADIANCE_TEXTURE, RADIANCE_TEXTURE, DIRECTIONAL_LIGHT, POINT_LIGHT
	};

	enum class UniformType
	{
		NONE = -1, BOOL, INT, FLOAT, FLOAT2, FLOAT3, FLOAT4, MAT4, TEXTURE_2D, TEXTURE_CUBE
	};

	struct ShaderUniform
	{
		ShaderUniform(const std::string name, UniformType type, uint32_t size, uint32_t offset, uint32_t sampler, RendererUniformID id)
			: Name(name), Type(type), Size(size), Offset(offset), Sampler(sampler), ID(id)
		{
		}

		const std::string Name;
		UniformType Type;
		RendererUniformID ID;
		uint32_t Size;
		uint32_t Offset;
		uint32_t Sampler;
	};

	class Shader : public Asset
	{
	public:
		Shader(const std::string& path);

		void Bind();
		void Unbind();
		bool Reload(); // This function is not implemented as of now

		inline const std::string& GetPath() const { return m_Path; }
		inline uint32_t GetShaderProgram() const { return m_ShaderProgram; }

		inline const std::unordered_map<RendererUniformID, Ref<ShaderUniform>>& GetRendererUniforms() const { return m_RendererUniforms; }
		inline const std::unordered_map<std::string, Ref<ShaderUniform>>& GetMaterialUniforms() const { return m_MaterialUniforms; }

		inline uint32_t GetMaterialUniformBufferSize() { return m_MaterialUniformBufferSize; }
		static uint32_t GetUniformSizeFromType(UniformType type);

		void SetUniformInt(const std::string& name, int value);
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
		void Init();

		std::unordered_map<ShaderStage, std::string> SplitShaders(const std::string& path);
		uint32_t CompileShaders(const std::unordered_map<ShaderStage, std::string>& shaderSrc);
		void ShaderProgramReflect();

		shaderc_shader_kind ShaderStageToShaderc(ShaderStage type);
		GLenum ShaderStageToGL(ShaderStage type);
		UniformType UniformTypeFromGL(GLenum type);
		RendererUniformID GetRendererUniformID(const std::string name);

	private:
		std::string m_Path;
		uint32_t m_ShaderProgram;

		std::unordered_map<ShaderStage, std::string> m_ShaderSrc;

		std::unordered_map<RendererUniformID, Ref<ShaderUniform>> m_RendererUniforms;
		std::unordered_map<std::string, Ref<ShaderUniform>> m_MaterialUniforms;
		uint32_t m_MaterialUniformBufferSize = 0;
	};

}