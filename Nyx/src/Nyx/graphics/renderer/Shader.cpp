#include "NXpch.h"
#include "Shader.h"
#include "glad/glad.h"
#include <glm/gtc/type_ptr.hpp>
#include "Nyx/Utilities.h"

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
		NX_CORE_ASSERT(m_ShaderID, "Shader ID is NULL (Shader most likely failed to compile)");
		NX_CORE_INFO("Created Shader at Path: {0}", m_FilePath);

		ParseUniforms();

		PrintUniforms();
	}

	Shader::~Shader()
	{
		glDeleteProgram(m_ShaderID);
	}

	uint Shader::Init()
	{
		uint program = glCreateProgram();
		uint vertex = glCreateShader(GL_VERTEX_SHADER);
		uint fragment = glCreateShader(GL_FRAGMENT_SHADER);

		ShaderSource shaderSrc = SplitShader(m_FilePath);

		String vertexSrc = shaderSrc.VertexSource;
		String fragSrc = shaderSrc.FragmentSource;

		if (vertexSrc == "" || fragSrc == "")
		{
			NX_CORE_ASSERT(false, "Vertex or Fragment shader is empty!");
			return 0;
		}

		if (CompileShader(vertex, vertexSrc) && CompileShader(fragment, fragSrc))
		{
			glAttachShader(program, vertex);
			glAttachShader(program, fragment);

			glLinkProgram(program);
			glValidateProgram(program);

			glDeleteShader(vertex);
			glDeleteShader(fragment);

			return program;
		}

		return 0;
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
		const char* shaderString = shaderSrc.c_str();

		glShaderSource(shader, 1, &shaderString, NULL);
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
	}

	void Shader::PushUniform(ShaderType uniform)
	{
		Bind();
		ShaderUniform* shaderUniform;
		if (uniform.type == "sampler2D" || uniform.type == "samplerCube") 
		{
			shaderUniform = new ShaderUniform(uniform.name, uniform.type, 1, m_Sampler);
			SetUniform1i(shaderUniform->GetName(), shaderUniform->GetSampler());
			m_Sampler++;
		}
		else 
		{
			shaderUniform = new ShaderUniform(uniform.name, uniform.type, 1, -1);
		}

		if (m_UserUniforms.size() > 0 || m_RenderUniforms.size() > 0) 
		{
			shaderUniform->SetOffset(m_UniformSize);
		}

		m_UniformSize += shaderUniform->GetSize();
		
		if (shaderUniform->GetName().front() == 'u')
		{
			m_UniformUserSize += shaderUniform->GetSize();
			m_UserUniforms.push_back(shaderUniform);
		}
		else if (shaderUniform->GetName().front() == 'r')
		{
			m_UniformRenderSize += shaderUniform->GetSize();
			m_RenderUniforms.push_back(shaderUniform);
		}
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
		for each (ShaderUniform* uniform in m_UserUniforms)
		{
			NX_CORE_DEBUG("Name: {0}", uniform->GetName());
			NX_CORE_DEBUG("	Type:    {0}", uniform->GetTypeString());
			NX_CORE_DEBUG("	Size:    {0}", uniform->GetSize());
			NX_CORE_DEBUG("	Offset:  {0}", uniform->GetOffset());
			NX_CORE_DEBUG("	Sampler: {0}", uniform->GetSampler());
		}
	}

	ShaderUniform* Shader::FindRenderUniform(const String& name)
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

	ShaderUniform* Shader::FindUserUniform(const String & name)
	{
		for (int i = 0;i < m_UserUniforms.size();i++)
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

	void Shader::SetUniform4f(const String& name, const glm::vec4& vec)
	{
		glUniform4f(GetUniformLocation(name), vec.x, vec.y, vec.z, vec.w);
	}

	void Shader::SetUniformMat4(const String& name, const glm::mat4& matrix)
	{
		glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(matrix[0]));
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