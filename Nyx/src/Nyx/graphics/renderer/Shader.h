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

	public:
		void Bind();
		void Unbind();

		void Reload();

		void PrintUniforms();
		ShaderUniform* FindUniform(const String& name);
		inline int GetUniformSize() const { return m_UniformSize; }
		inline std::vector<ShaderUniform*> GetUniforms() const { return m_Uniforms; }
		
		inline uint GetID() const { return m_ShaderID; }
		inline const String& GetPath() const { return m_FilePath; }

		void SetUniform1f(const String& name, float value);
		void SetUniform2f(const String& name, const glm::vec2& vec);
		void SetUniform3f(const String& name, const glm::vec3& vec);
		void SetUniform4f(const String& name, const glm::vec4& vec);
		void SetUniformMat4(const String& name, const glm::mat4& matrix);
		void SetUniform1i(const String& name, int value);
		void SetUniform1iv(const String& name, int* value, int count);
		void SetUniformBool(const String& name, bool value);

	private:
		uint Init();
		ShaderSource SplitShader(const String& filePath);
		int CompileShader(uint shader, const String& shaderSrc);

		void ParseUniformStructs();
		void ParseUniformBlock(int startingLineNumber);
		void ParseUniforms();
		void PushUniform(ShaderType uniform);

		void SetTextureIDs(const String& name);
		int GetUniformLocation(const String& name);

	private:
		uint m_ShaderID;
		String m_FilePath;

		int m_Sampler = 0;
		int m_UniformSize = 0;

		std::unordered_map<String, int> m_UniformLocationCache;
		std::vector<UniformStruct> m_UniformStructs;
		std::vector<ShaderUniform*> m_Uniforms;
	};

}