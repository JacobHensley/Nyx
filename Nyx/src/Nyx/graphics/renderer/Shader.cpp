#include "NXpch.h"
#include "Shader.h"
#include "glad/glad.h"
#include <glm/gtc/type_ptr.hpp>
#include "Nyx/Utilities.h"

#include <shaderc/shaderc.hpp>
#include "spirv_reflect.hpp"

namespace Nyx {

	struct ShaderSource
	{
		String VertexSource;
		String FragmentSource;
	};

	Shader::Shader(const String& filePath)
		: m_FilePath(filePath)
	{
		m_ShaderID = Init();
	//	NX_CORE_ASSERT(m_ShaderID, "Shader ID is NULL (Shader most likely failed to compile)");
		NX_CORE_INFO("Created Shader at Path: {0}", m_FilePath);

		ParseUniforms();
		PrintUniforms();

		// Renderer Uniforms
		ResolveRendererUniform(RenderUniformID::MODEL_MATRIX, "r_ModelMatrix");
		ResolveRendererUniform(RenderUniformID::VIEW_MATRIX, "r_ViewMatrix");
		ResolveRendererUniform(RenderUniformID::PROJ_MATRIX, "r_ProjMatrix");
		ResolveRendererUniform(RenderUniformID::INVERSE_VP, "r_InverseVP");
		ResolveRendererUniform(RenderUniformID::MVP, "r_MVP");
		ResolveRendererUniform(RenderUniformID::CAMERA_POSITION, "r_CameraPosition");
	}

	Shader::~Shader()
	{
		glDeleteProgram(m_ShaderID);
	}

	uint Shader::Init()
	{
	//	uint program = glCreateProgram();
	//	uint vertex = glCreateShader(GL_VERTEX_SHADER);
	//	uint fragment = glCreateShader(GL_FRAGMENT_SHADER);

		ShaderSource shaderSrc = SplitShader(m_FilePath);

		if (shaderSrc.VertexSource == "" || shaderSrc.FragmentSource == "")
		{
			NX_CORE_ASSERT(false, "Vertex or Fragment shader is empty!");
			return 0;
		}


		CompileShaders(shaderSrc);

		return 0;
	}

	void Shader::CompileShaders(ShaderSource src)
	{
		shaderc::Compiler compiler;
		shaderc::CompileOptions options;

		options.SetTargetEnvironment(shaderc_target_env_opengl, 430);

		shaderc::SpvCompilationResult vertexResult = compiler.CompileGlslToSpv(src.VertexSource, shaderc_glsl_vertex_shader, (m_FilePath + " (Vertex)").c_str(), options);
		shaderc::SpvCompilationResult fragmentResult = compiler.CompileGlslToSpv(src.FragmentSource, shaderc_glsl_fragment_shader, (m_FilePath + " (Fragment)").c_str(), options);

		if (vertexResult.GetCompilationStatus() == shaderc_compilation_status_compilation_error)
			NX_CORE_ERROR("Warnings ({0}), Errors ({1}) \n{2}", vertexResult.GetNumWarnings(), vertexResult.GetNumErrors(), vertexResult.GetErrorMessage());

		if (fragmentResult.GetCompilationStatus() == shaderc_compilation_status_compilation_error)
			NX_CORE_ERROR("Warnings ({0}), Errors ({1}) \n{2}", fragmentResult.GetNumWarnings(), fragmentResult.GetNumErrors(), fragmentResult.GetErrorMessage());

		std::vector<uint32_t> spirvVertex = { vertexResult.cbegin(), vertexResult.cend()};
		std::vector<uint32_t> spirvFragment = { fragmentResult.cbegin(), fragmentResult.cend() };
		
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderBinary(1, &vertexShader, GL_SHADER_BINARY_FORMAT_SPIR_V, spirvVertex.data(), spirvVertex.size());
		glSpecializeShader(vertexShader, (const GLchar*)"main", 0, nullptr, nullptr);

		GLint isCompiled = 0;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);

			glDeleteShader(vertexShader);
			return;
		}

		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderBinary(1, &fragmentShader, GL_SHADER_BINARY_FORMAT_SPIR_V, spirvFragment.data(), spirvFragment.size());
		glSpecializeShader(fragmentShader, (const GLchar*)"main", 0, nullptr, nullptr);

		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);

			glDeleteShader(fragmentShader);
			glDeleteShader(vertexShader);
			return;
		}

		// Vertex and fragment shaders are successfully compiled.
		// Now time to link them together into a program.
		// Get a program object.
		GLuint program = glCreateProgram();

		// Attach our shaders to our program
		glAttachShader(program, vertexShader);
		glAttachShader(program, fragmentShader);

		// Link our program
		glLinkProgram(program);

		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
			NX_CORE_ERROR("{0}", &infoLog[0]);
			// We don't need the program anymore.
			glDeleteProgram(program);
			// Don't leak shaders either.
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);

			// Use the infoLog as you see fit.

			// In this simple program, we'll just leave
			return;
		}

		// Always detach shaders after a successful link.
		glDetachShader(program, vertexShader);
		glDetachShader(program, fragmentShader);
	}

	ShaderSource Shader::SplitShader(const String& filePath)
	{
		std::ifstream stream(filePath);

		enum class ShaderType
		{
			NONE = -1, VERTEX = 0, FRAGMENT = 1
		};

		ShaderType type = ShaderType::NONE;
		std::stringstream ss[2];
		String line;

		int lineNumber = 1;

		while (getline(stream, line))
		{
			if (line.find("#Shader") != String::npos)
			{
				if (line.find("Vertex") != String::npos)
				{
					type = ShaderType::VERTEX;
				}
				else if (line.find("Fragment") != String::npos)
				{
					type = ShaderType::FRAGMENT;
				}
			}
			else
			{
				ss[(int)type] << line << '\n';

				if (line.find("#version") != String::npos)
				{
					ss[(int)type] << "#line " + std::to_string(lineNumber + 3) << '\n';
				}

				lineNumber++;
			}
		}

		return { ss[0].str(), ss[1].str() };

	}
	int Shader::CompileShader(uint shader, const String& shaderSrc)
	{
	//	glShaderSource(shader, 1, &shaderSrc, NULL);
		glCompileShader(shader);

		int compileResult;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compileResult);

		if (compileResult == GL_FALSE)
		{
			int length;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
			std::vector<char> error(length);
			glGetShaderInfoLog(shader, length, &length, &error[0]);

			NX_CORE_ERROR("{0}", &error[0]);
			glDeleteShader(shader);
			return 0;
		}

		return 1;
	}

	void Shader::ParseUniformStructs()
	{
		std::ifstream stream(m_FilePath);
		String line;

		bool inStuct = false;
		bool foundStruct = false;

		while (getline(stream, line))
		{
			if (line.find("}") != String::npos)
			{
				foundStruct = false;
				inStuct = false;
			}

			if (line.find("struct") != String::npos && line.find("//") == String::npos && line.empty() == false)
			{
				std::vector<String> tokens = Tokenize(line, ' ');
				m_UniformStructs.push_back(UniformStruct(tokens[1]));
				foundStruct = true;
			}

			if (inStuct == true)
			{
				std::vector<String> type = Tokenize(line, ' ');
				String name = type[1];
				name.pop_back();
				m_UniformStructs.back().AddType(type[0], name);
			}

			if (line.find("{") != String::npos && foundStruct == true)
			{
				inStuct = true;
				continue;
			}
		}
	}

	void Shader::ParseUniformBlock(int startingLineNumber)
	{
		std::ifstream stream(m_FilePath);
		String line;
		int lineNum = 0;

		while (getline(stream, line))
		{
			if (lineNum == startingLineNumber)
			{
				std::vector<String> tokens = Tokenize(line, ' ');
				m_UniformStructs.push_back(UniformStruct(tokens[1]));
			}

			if (lineNum - 2 >= startingLineNumber && line.find("//") == String::npos && line.empty() == false)
			{
				if (line.find("}") != String::npos)
				{
					return;
				}

				std::vector<String> type = Tokenize(line, ' ');
				String name = type[1];
				name.pop_back();

				String typeToken = type[0];
				typeToken.erase(std::remove(typeToken.begin(), typeToken.end(), '\t'), typeToken.end());

				m_UniformStructs.back().AddType(typeToken, name);
				PushUniform(ShaderType(m_UniformStructs.back().name + "." + name, typeToken));
			}
			lineNum++;
		}
	}

	void Shader::ParseUniforms()
	{
		std::ifstream stream(m_FilePath);
		String line;
		int lineNum = 0;

		std::vector<ShaderType> rawUniforms;

		while (getline(stream, line))
		{
			if (line.find("uniform") != String::npos && line.find("//") == String::npos)
			{
				std::vector<String> tokens = Tokenize(line, ' ');

				if (tokens.size() == 2)
				{
					ParseUniformBlock(lineNum);
					continue;
				}

				String name = tokens[2];
				name.pop_back();
				rawUniforms.push_back(ShaderType(name, tokens[1]));
			}
			lineNum++;
		}

		for (int i = 0; i < rawUniforms.size(); i++)
		{
			ShaderType uniform = rawUniforms[i];
			bool isInStruct = false;
			for (int j = 0; j < m_UniformStructs.size(); j++)
			{
				if (uniform.type == m_UniformStructs[j].name)
				{
					isInStruct = true;
					for (int k = 0; k < m_UniformStructs[j].types.size(); k++)
					{
						String type = m_UniformStructs[j].types[k].first;
						type.erase(std::remove(type.begin(), type.end(), '\t'), type.end());
						PushUniform(ShaderType(m_UniformStructs[j].name + "." + m_UniformStructs[j].types[k].second, type));
					}
				}
			}
			if (isInStruct == false)
			{
				PushUniform(rawUniforms[i]);
			}
			isInStruct = false;
		}

		SetRenderUniformIDs();
	}

	void Shader::SetRenderUniformIDs()
	{
		for (int i = 0; i < m_RenderUniforms.size(); i++)
		{
			const String& name = m_RenderUniforms.at(i)->GetName();
			if (name == "r_ModelMatrix")
				m_RenderUniformIDs.push_back(RenderUniformID::MODEL_MATRIX);
			if (name == "r_ViewMatrix")
				m_RenderUniformIDs.push_back(RenderUniformID::VIEW_MATRIX);
			if (name == "r_ProjMatrix")
				m_RenderUniformIDs.push_back(RenderUniformID::PROJ_MATRIX);
			if (name == "r_InverseVP")
				m_RenderUniformIDs.push_back(RenderUniformID::INVERSE_VP);
			if (name == "r_MVP")
				m_RenderUniformIDs.push_back(RenderUniformID::MVP);
			if (name == "r_CameraPosition")
				m_RenderUniformIDs.push_back(RenderUniformID::CAMERA_POSITION);
		}
	}

	void Shader::PushUniform(ShaderType uniform)
	{
		Bind();
		Ref<ShaderUniform> shaderUniform;
		if (uniform.type == "sampler2D" || uniform.type == "samplerCube")
		{
			shaderUniform = CreateRef<ShaderUniform>(uniform.name, uniform.type, 1, m_Sampler);
			SetUniform1i(shaderUniform->GetName(), shaderUniform->GetSampler());
			m_Sampler++;
		}
		else
		{
			shaderUniform = CreateRef<ShaderUniform>(uniform.name, uniform.type, 1, -1);
		}

		if (shaderUniform->GetName().front() == 'u')
		{
			m_UniformUserSize += shaderUniform->GetSize();
			m_UserUniforms.push_back(shaderUniform);

			shaderUniform->SetOffset(m_UserUniformBufferOffset);
			m_UserUniformBufferOffset += shaderUniform->GetSize();
		}
		else if (shaderUniform->GetName().front() == 'r')
		{
			m_UniformRenderSize += shaderUniform->GetSize();
			m_RenderUniforms.push_back(shaderUniform);

			shaderUniform->SetOffset(m_RendererUniformBufferOffset);
			m_RendererUniformBufferOffset += shaderUniform->GetSize();
		}

	}

	bool Shader::ResolveRendererUniform(RenderUniformID id, const std::string& name)
	{
		int location = GetUniformLocation(name);
		if (location != -1)
		{
			m_RendererUniforms.emplace(RendererUniform{ id, location });
			return true;
		}
		return false;
	}

	void Shader::SetTextureIDs(const String& name)
	{
		int textureIDs[32];
		for (int i = 0; i < 32; i++)
		{
			textureIDs[i] = i;
		}

		Bind();
		SetUniform1iv(name, textureIDs, 32);
		Unbind();
	}

	void Shader::Bind()
	{
		glUseProgram(m_ShaderID);
	}

	void Shader::Unbind()
	{
		glUseProgram(0);
	}

	void Shader::Reload()
	{
		m_UniformLocationCache.clear();
		uint reloadedShader = Init();

		if (reloadedShader == NULL)
		{
			NX_CORE_WARN("Shader failed to compile aborting shader reload!");
			return;
		}

		glDeleteProgram(m_ShaderID);
		m_UniformLocationCache.clear();
		m_Sampler = 0;
		m_UniformSize = 0;
		m_UserUniforms.clear();
		m_UniformStructs.clear();
		m_ShaderID = reloadedShader;

		ParseUniforms();
	}

	void Shader::PrintUniforms()
	{
		for each (Ref<ShaderUniform> uniform in m_UserUniforms)
		{
			NX_CORE_DEBUG("Name: {0}", uniform->GetName());
			NX_CORE_DEBUG("	Type:    {0}", uniform->GetTypeString());
			NX_CORE_DEBUG("	Size:    {0}", uniform->GetSize());
			NX_CORE_DEBUG("	Offset:  {0}", uniform->GetOffset());
			NX_CORE_DEBUG("	Sampler: {0}", uniform->GetSampler());
		}
	}

	Ref<ShaderUniform> Shader::FindRenderUniform(const String& name)
	{
		for (int i = 0; i < m_RenderUniforms.size(); i++)
		{
			if (m_RenderUniforms[i]->GetName() == name)
			{
				return m_RenderUniforms[i];
			}
		}

		return nullptr;
	}

	Ref<ShaderUniform> Shader::FindUserUniform(const String& name)
	{
		for (int i = 0; i < m_UserUniforms.size(); i++)
		{
			if (m_UserUniforms[i]->GetName() == name)
			{
				return m_UserUniforms[i];
			}
		}

		return nullptr;
	}

	int Shader::GetUniformLocation(const String& name)
	{
		if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
		{
			return m_UniformLocationCache[name];
		}

		int location = glGetUniformLocation(m_ShaderID, name.c_str());

		//	NX_CORE_ASSERT(location != -1, "Invalid uniform location");

		m_UniformLocationCache[name] = location;
		return location;
	}

	void Shader::SetUniform1f(const String& name, float value)
	{
		glUniform1f(GetUniformLocation(name), value);
	}

	void Shader::SetUniform2f(const String& name, const glm::vec2& vec)
	{
		glUniform2f(GetUniformLocation(name), vec.x, vec.y);
	}

	void Shader::SetUniform3f(const String& name, const glm::vec3& vec)
	{
		glUniform3f(GetUniformLocation(name), vec.x, vec.y, vec.z);
	}

	void Shader::SetUniform3f(int location, const glm::vec3& vec)
	{
		glUniform3f(location, vec.x, vec.y, vec.z);
	}

	void Shader::SetUniform4f(const String& name, const glm::vec4& vec)
	{
		glUniform4f(GetUniformLocation(name), vec.x, vec.y, vec.z, vec.w);
	}

	void Shader::SetUniformMat4(const String& name, const glm::mat4& matrix)
	{
		glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(matrix[0]));
	}

	void Shader::SetUniformMat4(int location, const glm::mat4& matrix)
	{
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix[0]));
	}

	void Shader::SetUniform1i(const String& name, int value)
	{
		glUniform1i(GetUniformLocation(name), value);
	}

	void Shader::SetUniform1iv(const String& name, int* value, int count)
	{
		glUniform1iv(GetUniformLocation(name), count, value);
	}

	void Shader::SetUniformBool(const String& name, bool value)
	{
		glUniform1i(GetUniformLocation(name), value);
	}

}