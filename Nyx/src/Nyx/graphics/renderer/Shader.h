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

	enum RenderUniformID
	{
		IDNONE = 0, MODEL_MATRIX, VIEW_MATRIX, PROJ_MATRIX, INVERSE_VP, MVP, CAMERA_POSITION
	};

	struct RendererUniform
	{
		RenderUniformID ID;
		int32_t Location;

		bool operator==(const RendererUniform& other) const { return ID == other.ID; }
	};

	struct RendererUniformHash {
		size_t operator() (const RendererUniform& uniform) const { return std::hash<int>()(uniform.ID); }
	};

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
		using RendererUniformSet = std::unordered_set<RendererUniform, RendererUniformHash>;
	public:
		Shader(const String& filePath);
		~Shader();

	public:
		void Bind();
		void Unbind();

		void Reload();

		void PrintUniforms();

		Ref<ShaderUniform> FindRenderUniform(const String& name);
		Ref<ShaderUniform> FindUserUniform(const String& name);
		
		inline int GetUserUniformSize() const { return m_UniformUserSize; }
		inline std::vector<Ref<ShaderUniform>> GetRenderUniforms() const { return m_RenderUniforms; }

		inline int GetRenderUniformSize() const { return m_UniformRenderSize; }
		inline std::vector<Ref<ShaderUniform>> GetUserUniforms() const { return m_UserUniforms; }
		
		inline std::vector<RenderUniformID> GetRenderUniformIDs() const { return m_RenderUniformIDs; }

		inline uint GetID() const { return m_ShaderID; }
		inline const String& GetPath() const { return m_FilePath; }

		const RendererUniformSet& GetRendererUniforms() const { return m_RendererUniforms; }

		void SetUniform1f(const String& name, float value);
		void SetUniform2f(const String& name, const glm::vec2& vec);
		void SetUniform3f(const String& name, const glm::vec3& vec);
		void SetUniform3f(int location, const glm::vec3& vec);
		void SetUniform4f(const String& name, const glm::vec4& vec);
		void SetUniformMat4(const String& name, const glm::mat4& matrix);
		void SetUniformMat4(int location, const glm::mat4& matrix);
		void SetUniform1i(const String& name, int value);
		void SetUniform1iv(const String& name, int* value, int count);
		void SetUniformBool(const String& name, bool value);

		std::string GetSuffix(const std::string& name);
		EShLanguage GetShaderStage(const std::string& stage);

	private:
		uint Init();
		ShaderSource SplitShader(const String& filePath);
		int CompileShader(uint shader, const String& shaderSrc);

		void ParseUniformStructs();
		void ParseUniformBlock(int startingLineNumber);
		void ParseUniforms();
		void SetRenderUniformIDs();
		void PushUniform(ShaderType uniform);
		bool ResolveRendererUniform(RenderUniformID id, const std::string& name);

		void SetTextureIDs(const String& name);
		int GetUniformLocation(const String& name);

	private:
		uint m_ShaderID;
		String m_FilePath;

		int m_Sampler = 0;
		int m_UniformSize = 0;

		uint32_t m_UserUniformBufferOffset = 0;
		uint32_t m_RendererUniformBufferOffset = 0;

		int m_UniformRenderSize = 0;
		int m_UniformUserSize = 0;

		std::unordered_map<String, int> m_UniformLocationCache;
		std::vector<RenderUniformID> m_RenderUniformIDs;
		RendererUniformSet m_RendererUniforms;
		std::vector<UniformStruct> m_UniformStructs;
		std::vector<Ref<ShaderUniform>> m_UserUniforms;
		std::vector<Ref<ShaderUniform>> m_RenderUniforms;
	};

}