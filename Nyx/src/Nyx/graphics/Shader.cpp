#include "NXpch.h"
#include "Shader.h"
#include "Nyx/Utilities/Utilities.h"
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

namespace Nyx {

	namespace Utils {
	
		static shaderc_shader_kind ShaderStageToShaderc(ShaderStage stage)
		{
			switch (stage)
			{
				case ShaderStage::VERTEX:	return shaderc_vertex_shader;
				case ShaderStage::FRAGMENT: return shaderc_fragment_shader;
				case ShaderStage::COMPUTE: return shaderc_compute_shader;
			}

			NX_CORE_ASSERT(false, "Unknown Type");
			return (shaderc_shader_kind)0;
		}

		static GLenum ShaderStageToGL(ShaderStage stage)
		{
			switch (stage)
			{
				case ShaderStage::VERTEX:	return GL_VERTEX_SHADER;
				case ShaderStage::FRAGMENT: return GL_FRAGMENT_SHADER;
				case ShaderStage::COMPUTE: return GL_COMPUTE_SHADER;
			}

			NX_CORE_ASSERT(false, "Unknown Type");
			return 0;
		}

		static UniformType UniformTypeFromGL(GLenum type)
		{
			switch (type)
			{
				case GL_BOOL:		  return UniformType::BOOL;
				case GL_INT:		  return UniformType::INT;
				case GL_FLOAT:		  return UniformType::FLOAT;
				case GL_FLOAT_VEC2:	  return UniformType::FLOAT2;
				case GL_FLOAT_VEC3:	  return UniformType::FLOAT3;
				case GL_FLOAT_VEC4:	  return UniformType::FLOAT4;
				case GL_FLOAT_MAT4:	  return UniformType::MAT4;
				case GL_SAMPLER_2D:	  return UniformType::TEXTURE_2D;
				case GL_SAMPLER_CUBE: return UniformType::TEXTURE_CUBE;
				case GL_IMAGE_2D:     return UniformType::IMAGE_2D;
			}

			NX_CORE_ASSERT(false, "Unknown Type");
			return UniformType::NONE;
		}
	}

	Shader::Shader(const std::string& path)
		:	m_Path(path)
	{
		Init();
	}

	bool Shader::Init()
	{
		m_ShaderSrc = SplitShaders(m_Path);
		bool compileResult = CompileShaders(m_ShaderSrc);
		ShaderProgramReflect();

		m_Initialized = true;
		return compileResult;
	}

	bool Shader::CompileShaders(const std::unordered_map<ShaderStage, std::string>& shaderSrc)
	{
		shaderc::Compiler compiler;
		shaderc::CompileOptions options;

		const int shaderVersion = 450;
		options.SetTargetEnvironment(shaderc_target_env_opengl, shaderVersion);

		GLuint program = glCreateProgram();
		std::vector<GLuint> shaderIDs(shaderSrc.size());
		uint32_t index = 0;
		
		for (auto&& [stage, src] : shaderSrc)
		{
			auto compilationResult = compiler.CompileGlslToSpv(src, Utils::ShaderStageToShaderc(stage), m_Path.c_str(), options);

			if (compilationResult.GetCompilationStatus() != shaderc_compilation_status_success)
			{
				NX_CORE_ERROR("Warnings ({0}), Errors ({1}) \n{2}", compilationResult.GetNumWarnings(), compilationResult.GetNumErrors(), compilationResult.GetErrorMessage());
				NX_CORE_ASSERT(m_Initialized, "");
				return false;
			}

			const uint8_t* data = reinterpret_cast<const uint8_t*>(compilationResult.cbegin());
			const uint8_t* dataEnd = reinterpret_cast<const uint8_t*>(compilationResult.cend());
			uint32_t size = dataEnd - data;

			GLuint shader = glCreateShader(Utils::ShaderStageToGL(stage));
			glShaderBinary(1, &shader, GL_SHADER_BINARY_FORMAT_SPIR_V, data, size);
			glSpecializeShader(shader, (const GLchar*)"main", 0, nullptr, nullptr);

			shaderIDs[index++] = shader;
			glAttachShader(program, shader);
		}

		glLinkProgram(program);

		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);

			if (infoLog.size() > 0)
			{
				glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
				NX_CORE_ASSERT(m_Initialized, &infoLog[0]);
				return false;
			}

			glDeleteProgram(program);
		}

		for (GLuint shader : shaderIDs)
		{
			glDetachShader(program, shader);
		}

		m_ShaderProgram =  program;
		return true;
	}

	//Add #Line as to reset error message line number for each shader stage
	std::unordered_map<ShaderStage, std::string> Shader::SplitShaders(const std::string& path)
	{
		std::ifstream stream(path);

		std::unordered_map<ShaderStage, std::string> result;

		ShaderStage stage = ShaderStage::NONE;
		std::stringstream ss[2];
		std::string line;

		while (getline(stream, line))
		{
			if (line.find("#Shader") != std::string::npos)
			{
				if (line.find("Vertex") != std::string::npos)
				{
					stage = ShaderStage::VERTEX;
				}
				else if (line.find("Fragment") != std::string::npos)
				{
					stage = ShaderStage::FRAGMENT;
				}
				else if (line.find("Compute") != std::string::npos)
				{
					stage = ShaderStage::COMPUTE;
				}
			}
			else
			{
				result[stage] += line + '\n';
			}
		}

		return result;
	}

	void Shader::ShaderProgramReflect()
	{
		GLint numBlocks;
		glGetProgramiv(m_ShaderProgram, GL_ACTIVE_UNIFORM_BLOCKS, &numBlocks);
		std::vector<std::string> blockNames;
		blockNames.reserve(numBlocks);

		for (int i = 0; i < numBlocks; i++)
		{
			GLint nameLength;
			glGetActiveUniformBlockiv(m_ShaderProgram, i, GL_UNIFORM_BLOCK_NAME_LENGTH, &nameLength);

			std::vector<GLchar> name;
			name.resize(nameLength);
			glGetActiveUniformBlockName(m_ShaderProgram, i, nameLength, NULL, &name[0]);

			blockNames.push_back(std::string());
			blockNames.back().assign(name.begin(), name.end() - 1);
		}

		const GLsizei nameBufferSize = 128;
		GLsizei nameLength;
		GLchar name[nameBufferSize];
		GLint size;
		GLenum type;

		uint32_t materialUniformOffset = 0;
		uint32_t rendererUniformOffset = 0;
		uint32_t resourceTextureUnit = 0;

		int numUniforms;
		glGetProgramiv(m_ShaderProgram, GL_ACTIVE_UNIFORMS, &numUniforms);
		for (int i = 0; i < numUniforms; i++)
		{
			glGetActiveUniform(m_ShaderProgram, (GLuint)i, nameBufferSize, &nameLength, &size, &type, name);

			const std::string strName = name;
			size_t n = std::count(strName.begin(), strName.end(), '.');
			if (n > 2)
				continue;

			UniformType uniformType = Utils::UniformTypeFromGL(type);

			if (strName.find("u_Material") != std::string::npos)
			{
				NX_CORE_ASSERT(m_MaterialUniforms.find(strName) == m_MaterialUniforms.end(), "Uniform already exists");

				uint32_t uniformSize = GetUniformSizeFromType(uniformType);
				m_MaterialUniforms[strName] = ShaderUniform(strName, uniformType, uniformSize, materialUniformOffset);
				materialUniformOffset += uniformSize;
				m_MaterialUniformBufferSize += uniformSize;

				continue;
			}

			if (strName.find("r_Renderer") != std::string::npos)
			{
				NX_CORE_ASSERT(m_RendererUniforms.find(strName) == m_RendererUniforms.end(), "Uniform already exists");

				uint32_t uniformSize = GetUniformSizeFromType(uniformType);
				m_RendererUniforms[strName] = ShaderUniform(strName, uniformType, uniformSize, rendererUniformOffset);
				rendererUniformOffset += uniformSize;

				continue;
			}

			if (uniformType == UniformType::TEXTURE_2D || uniformType == UniformType::TEXTURE_CUBE)
			{
				NX_CORE_ASSERT(m_Resources.find(strName) == m_Resources.end(), "Resource already exists");

				m_Resources[strName] = ShaderResource(strName, uniformType, resourceTextureUnit);

				if (strName.find("u_") != std::string::npos) 
				{
					GLint location = glGetUniformLocation(m_ShaderProgram, strName.c_str());
					NX_CORE_ASSERT(location != -1, "Cannot find Resource");
					glProgramUniform1i(m_ShaderProgram, location, resourceTextureUnit);
				}

				resourceTextureUnit++;
				continue;
			}

		}

	}

	uint32_t Shader::GetUniformSizeFromType(UniformType type)
	{
		switch (type) 
		{
			case UniformType::BOOL:	  return 4;
			case UniformType::INT:	  return 4;
			case UniformType::FLOAT:  return 4;
			case UniformType::FLOAT2: return 4 * 2;
			case UniformType::FLOAT3: return 4 * 3;
			case UniformType::FLOAT4: return 4 * 4;
			case UniformType::MAT4:	  return 64;
		}

		NX_CORE_ASSERT(false, "Unknown Type");
		return -1;
	}

	void Shader::Bind()
	{
		glUseProgram(m_ShaderProgram);
	}

	void Shader::Unbind()
	{
		glUseProgram(0);
	}

	// Works... but suspiciously
	bool Shader::Reload()
	{
		m_RendererUniforms.clear();
		m_MaterialUniforms.clear();
		m_Resources.clear();

		bool result = Init();
		if (result)
			NX_CORE_INFO("Successfully reloaded shader");
		else
			NX_CORE_WARN("Failed to reload shader");

		return result;
	}

	uint32_t Shader::SetUniformInt(const std::string& name, int value)
	{
		GLint location = glGetUniformLocation(m_ShaderProgram, name.c_str());
		glUniform1i(location, value);
		return location;
	}

	void Shader::SetUniformIntArray(const std::string& name, int* values, uint32_t count)
	{
		GLint location = glGetUniformLocation(m_ShaderProgram, name.c_str());
		glUniform1iv(location, count, values);
	}

	void Shader::SetUniformFloat(const std::string& name, float value)
	{
		GLint location = glGetUniformLocation(m_ShaderProgram, name.c_str());
		glUniform1f(location, value);
	}

	void Shader::SetUniformFloat2(const std::string& name, const glm::vec2& value)
	{
		GLint location = glGetUniformLocation(m_ShaderProgram, name.c_str());
		glUniform2f(location, value.x, value.y);
	}

	void Shader::SetUniformFloat3(const std::string& name, const glm::vec3& value)
	{
		GLint location = glGetUniformLocation(m_ShaderProgram, name.c_str());
		glUniform3f(location, value.x, value.y, value.z);
	}

	void Shader::SetUniformFloat4(const std::string& name, const glm::vec4& value)
	{
		GLint location = glGetUniformLocation(m_ShaderProgram, name.c_str());
		glUniform4f(location, value.x, value.y, value.z, value.w);
	}

	void Shader::SetUniformMat4(const std::string& name, const glm::mat4& matrix)
	{
		GLint location = glGetUniformLocation(m_ShaderProgram, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

}