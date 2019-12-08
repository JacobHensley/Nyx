#pragma once
#include "Nyx.h"
#include "NXpch.h"
#include "ShaderUniform.h"
#include <glm/glm.hpp>

namespace Nyx {

	struct ShaderSource;

	struct UniformStruct
	{
		String name;
		std::vector<std::pair<String, String>> types;

		UniformStruct()
		{
		}

		UniformStruct(const String& name)
			: name(name)
		{
		}

		void AddType(const String& type, const String& name)
		{
			types.push_back(std::make_pair(type, name));
		}
	};

	struct ShaderType
	{
		String name;
		String type;

		ShaderType(const String& name, const String& type)
			: name(name), type(type)
		{
		}
	};

	class Shader
	{
	public:
		Shader(const String& filePath);
		~Shader();

		void Bind();
		void Unbind();

		void Reload();

		void ParseUniformStructs();
		void ParseUniforms();
		void ParseUniformBlock(int startingLineNumber);

		void Shader::PushUniform(ShaderType uniform);

		void SetUniformBuffer(byte* buffer, uint size);
		ShaderUniform* FindUniform(const String& name);

		void SetTextureIDs(const String& name);
		void PrintUniforms();
		int GetUniformLocation(const String& name);
		inline int GetUniformSize() const { return m_UniformSize; }
		inline std::vector<ShaderUniform*> GetUniforms() const { return m_Uniforms; }
		inline uint GetID() const { return m_ShaderID; }

		std::vector<String> Tokenize(const String& str, const char delimiter);

		void SetUniform1i(const String& name, int value);
		void SetUniform1iv(const String& name, int* value, int count);
		void SetUniform1f(const String& name, float value);
		void SetUniform2f(const String& name, const glm::vec2& vec);
		void SetUniform3f(const String& name, const glm::vec3& vec);
		void SetUniformVec4(const String& name, const glm::vec4& vec);
		void SetUniformBool(const String& name, bool value);

		void SetUniform4f(const String& name, float x, float y, float z, float w);
		void SetUniformMat4(const String& name, const glm::mat4& matrix);
	private:
		uint m_ShaderID;
		String m_FilePath;

		uint Init();
		ShaderSource SplitShader(const String& filePath);
		int CompileShader(uint shader, const String& shaderSrc);

		int m_Sampler = 0;
		std::vector<UniformStruct> m_UniformStructs;
		std::vector<ShaderUniform*> m_Uniforms;
		int m_UniformSize = 0;

		std::unordered_map<String, int> m_UniformLocationCache;
	};

}