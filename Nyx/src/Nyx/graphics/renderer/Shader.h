#pragma once
#include "Nyx.h"
#include "NXpch.h"
#include "ShaderUniform.h"
#include <glm/glm.hpp>

#include "glslang/public/ShaderLang.h"
#include <SPIRV/GlslangToSpv.h>
#include <StandAlone/DirStackFileIncluder.h>
#include "glslang/MachineIndependent/reflection.h"

namespace Nyx {

	struct ShaderSource;

	class Shader
	{
	public:
		Shader(const String& filePath);
		~Shader();

	public:
		void Bind();
		void Unbind();

		void Reload();

		inline std::vector<Ref<ShaderUniform>> GetRendererUniforms() const { return m_RendererUniforms; }
		Ref<ShaderUniform> FindRendererUniform(const String& name);
		inline int GetRendererUniformSize() const { return m_UniformRendererSize; }

		inline std::vector<Ref<ShaderUniform>> GetUserUniforms() const { return m_UserUniforms; }
		Ref<ShaderUniform> FindUserUniform(const String& name);
		inline int GetUserUniformSize() const { return m_UniformUserSize; }

		inline uint GetID() const { return m_ShaderID; }
		inline const String& GetPath() const { return m_FilePath; }

		void SetUniform1f(const String& name, float value);
		void SetUniform1f(int location, float value);

		void SetUniform2f(const String& name, const glm::vec2& vec);
		void SetUniform2f(int location, const glm::vec2& vec);

		void SetUniform3f(const String& name, const glm::vec3& vec);
		void SetUniform3f(int location, const glm::vec3& vec);

		void SetUniform4f(const String& name, const glm::vec4& vec);
		void SetUniform4f(int location, const glm::vec4& vec);

		void SetUniformMat4(const String& name, const glm::mat4& matrix);
		void SetUniformMat4(int location, const glm::mat4& matrix);

		void SetUniform1i(const String& name, int value);
		void SetUniform1i(int location, int value);

		void SetUniform1iv(const String& name, int* value, int count);
		void SetUniform1iv(int location, int* value, int count);

	private:
		uint Init();
		ShaderSource SplitShader(const String& filePath);
		int CompileShaders(uint shader, const String& shaderSrc);

		void SetTextureIDs(const String& name);

	private:
		uint m_ShaderID;
		String m_FilePath;

		std::vector<Ref<ShaderUniform>> m_UserUniforms;
		uint m_UserUniformBufferOffset = 0;
		uint m_UniformUserSize = 0;

		std::vector<Ref<ShaderUniform>> m_RendererUniforms;
		uint m_RendererUniformBufferOffset = 0;
		uint m_UniformRendererSize = 0;
		
	};

}