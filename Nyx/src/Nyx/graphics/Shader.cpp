#include "NXpch.h"
#include "Shader.h"
#include "Nyx/Utilities/Utilities.h"
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

namespace Nyx {

	Shader::Shader(const std::string& path)
		:	m_Path(path)
	{
		Init();
	}

	void Shader::Init()
	{
		m_ShaderSrc = SplitShaders(m_Path);
		m_ShaderProgram = CompileShaders(m_ShaderSrc);
		ShaderProgramReflect();
	}

	uint32_t Shader::CompileShaders(const std::unordered_map<ShaderStage, std::string>& shaderSrc)
	{
		shaderc::Compiler compiler;
		shaderc::CompileOptions options;

		const int shaderVersion = 430;
		options.SetTargetEnvironment(shaderc_target_env_opengl, shaderVersion);

		GLuint program = glCreateProgram();
		GLuint shaderIDs[2];
		uint32_t index = 0;

		for (auto const& shader : shaderSrc)
		{
			ShaderStage stage = shader.first;
			const std::string& src = shader.second;

			auto compilationResult = compiler.CompileGlslToSpv(src, ShaderStageToShaderc(stage), m_Path.c_str(), options);
			if (compilationResult.GetCompilationStatus() != shaderc_compilation_status_success)
			{
				NX_CORE_ERROR("Warnings ({0}), Errors ({1}) \n{2}", compilationResult.GetNumWarnings(), compilationResult.GetNumErrors(), compilationResult.GetErrorMessage());
			}

			const uint8_t* data = reinterpret_cast<const uint8_t*>(compilationResult.cbegin());
			const uint8_t* dataEnd = reinterpret_cast<const uint8_t*>(compilationResult.cend());
			uint32_t size = dataEnd - data;

			GLuint shader = glCreateShader(ShaderStageToGL(stage));
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
				NX_CORE_ERROR("{0}", &infoLog[0]);
			}

			glDeleteProgram(program);
		}

		for (GLuint shader : shaderIDs)
		{
			glDetachShader(program, shader);
		}

		return program;
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
			}
			else
			{
				result[stage] += line + '\n';
			}
		}

		return result;
	}

	//May have to adjust sizing and offset for bools
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
		GLchar name[nameBufferSize];
		GLint size;
		GLenum type;
		uint32_t offset = 0;
		uint32_t sampler = 0;
		GLsizei nameLength;

		int numUniforms;
		glGetProgramiv(m_ShaderProgram, GL_ACTIVE_UNIFORMS, &numUniforms);

		for (int i = 0; i < numUniforms; i++)
		{
			bool uniformInBlock = false;
			glGetActiveUniform(m_ShaderProgram, (GLuint)i, nameBufferSize, &nameLength, &size, &type, name);
			const std::string strName = name;
			size_t n = std::count(strName.begin(), strName.end(), '.');

			if (n >= 2)
			{
				continue;
			}

			RendererUniformID id = GetRendererUniformID(name);
			UniformType uniformType = UniformTypeFromGL(type);
			uint32_t uniformSize = GetUniformSizeFromType(uniformType);
			Ref<ShaderUniform> uniform = CreateRef<ShaderUniform>(name, uniformType, uniformSize, offset, sampler, id);
			if (uniform->ID != RendererUniformID::NONE)
			{
				m_RendererUniforms[id] = uniform;
			}
			else
			{
				m_MaterialUniforms[name] = uniform;
			}

			sampler += 1;
			offset += size;
			m_MaterialUniformBufferSize += size;
		}

	}

	shaderc_shader_kind Shader::ShaderStageToShaderc(ShaderStage type)
	{
		if (type == ShaderStage::VERTEX)
		{
			return shaderc_vertex_shader;
		}
		else if (type == ShaderStage::FRAGMENT)
		{
			return shaderc_fragment_shader;
		}
	}

	GLenum Shader::ShaderStageToGL(ShaderStage type)
	{
		if (type == ShaderStage::VERTEX)
		{
			return GL_VERTEX_SHADER;
		}
		else if (type == ShaderStage::FRAGMENT)
		{
			return GL_FRAGMENT_SHADER;
		}
	}

	UniformType Shader::UniformTypeFromGL(GLenum type)
	{
		if (type == GL_BOOL)
		{
			return UniformType::BOOL;
		}
		else if (type == GL_INT)
		{
			return UniformType::INT;
		}
		else if (type == GL_FLOAT)
		{
			return UniformType::FLOAT;
		}
		else if (type == GL_FLOAT_VEC2)
		{
			return UniformType::FLOAT2;
		}
		else if (type == GL_FLOAT_VEC3)
		{
			return UniformType::FLOAT3;
		}
		else if (type == GL_FLOAT_VEC4)
		{
			return UniformType::FLOAT4;
		}
		else if (type == GL_FLOAT_MAT4)
		{
			return UniformType::MAT4;
		}
		else if (type == GL_SAMPLER_2D)
		{
			return UniformType::TEXTURE_2D;
		}
		else if (type == GL_SAMPLER_CUBE)
		{
			return UniformType::TEXTURE_CUBE;
		}
		else
		{
			return UniformType::NONE;
		}
	}

	RendererUniformID Shader::GetRendererUniformID(const std::string name)
	{
		if (name == "r_Transform")
		{
			return RendererUniformID::TRANSFORM;
		}
		else if (name == "r_ViewMatrix")
		{
			return RendererUniformID::VIEW_MATRIX;
		}
		else if (name == "r_ViewProjection")
		{
			return RendererUniformID::VIEW_PROJECTION;
		}
		else if (name == "r_ProjMatrix")
		{
			return RendererUniformID::PROJECTION_MATRIX;
		}
		else if (name == "r_InverseVP")
		{
			return RendererUniformID::INVERSE_VP;
		}
		else if (name == "r_MVP")
		{
			return RendererUniformID::MVP;
		}
		else if (name == "r_CameraPosition")
		{
			return RendererUniformID::CAMERA_POSITION;
		}
		else if (name == "r_BRDFLutTexture")
		{
			return RendererUniformID::BRDF_LUT;
		}
		else if (name == "r_IrradianceTexture")
		{
			return RendererUniformID::IRRADIANCE_TEXTURE;
		}
		else if (name == "r_RadianceTexture")
		{
			return RendererUniformID::RADIANCE_TEXTURE;
		}
		else if (name.find("r_DirectionLight") != std::string::npos)
		{
			return RendererUniformID::DIRECTIONAL_LIGHT;
		}
		else if (name.find("r_PointLight") != std::string::npos)
		{
			return RendererUniformID::POINT_LIGHT;
		}

		return RendererUniformID::NONE;
	}

	uint32_t Shader::GetUniformSizeFromType(UniformType type)
	{
		if (type == UniformType::BOOL)
		{
			return 1;
		}
		else if (type == UniformType::INT)
		{
			return 4;
		}
		else if (type == UniformType::FLOAT)
		{
			return 4;
		}
		else if (type == UniformType::FLOAT2)
		{
			return 4 * 2;
		}
		else if (type == UniformType::FLOAT3)
		{
			return 4 * 3;
		}
		else if (type == UniformType::FLOAT4)
		{
			return 4 * 4;
		}
		else if (type == UniformType::MAT4)
		{
			return 64;
		}

		return 1;
	}

	void Shader::Bind()
	{
		glUseProgram(m_ShaderProgram);
	}

	void Shader::Unbind()
	{
		glUseProgram(0);
	}

	bool Shader::Reload()
	{
		return false;
	}

	void Shader::SetUniformInt(const std::string& name, int value)
	{
		GLint location = glGetUniformLocation(m_ShaderProgram, name.c_str());
		glUniform1i(location, value);
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