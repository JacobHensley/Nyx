#pragma once
#include "Nyx.h"
#include "NXpch.h"
#include "ShaderUniform.h"
#include <glm/glm.hpp>

namespace Nyx {

	struct ShaderSource;

	class Shader
	{
	public:
		Shader(const String& filePath);
		~Shader();

		void Bind();
		void Unbind();

		void Reload();

		void ParseUniforms();
		void PushUniform(ShaderUniform* uniform);
		void PrintUniforms();

		void SetTextureIDs(const String& name);
		int GetUniformLocation(const String& name);

		inline std::vector<ShaderUniform*>& GetUniforms() { return m_Uniforms; };
		inline uint GetID() const { return m_ShaderID; }

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

		std::vector<ShaderUniform*> m_Uniforms;
		std::unordered_map<String, int> m_UniformLocationCache;
	};

}