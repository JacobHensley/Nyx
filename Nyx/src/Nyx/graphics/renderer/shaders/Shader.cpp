#include "NXpch.h"
#include "Shader.h"
#include "glad/glad.h"
#include <glm/gtc/type_ptr.hpp>

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
		NX_CORE_ASSERT(m_ShaderID, "Shader ID is NULL");

		ParseUniforms();
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
			}
		}
		return { ss[0].str(), ss[1].str() };
	}

	int Shader::CompileShader(uint shader, const String& shaderSrc)
	{
		const char* vs = shaderSrc.c_str();

		glShaderSource(shader, 1, &vs, NULL);
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
			NX_CORE_ASSERT(false, "Shader failed to compile!");
			glDeleteShader(shader);
			return 0;
		}

		return 1;
	}

	void Shader::ParseUniforms()
	{
		std::ifstream stream(m_FilePath);

		String line;
		while (getline(stream, line))
		{
			if (line.find("uniform") != String::npos)
			{
				std::vector<String> tokens;
				String token;
				for (uint i = 0; i < line.length(); i++)
				{
					if (line[i] == '[')
					{
						tokens.push_back(token);
						token = "";
						uint last = (uint)line.find("]");
						token = line.substr(i + 1, last - i - 1);
						tokens.push_back(token);
						break;
					}

					if (line[i] == ';')
					{
						break;
					}

					if (line[i] == ' ')
					{
						tokens.push_back(token);
						token = "";
						continue;
					}
					token += line[i];
				}

				tokens.push_back(token);
				if (tokens.size() == 3)
					tokens.push_back("1");

				String& name = tokens[1];
				String& type = tokens[2];
				int count = std::stoi(tokens[3]);

				PushUniform(new ShaderUniform(type, name, count));
			}
		}
	}

	void Shader::PushUniform(ShaderUniform* uniform)
	{
		int offset = 0;
		if (m_Uniforms.size() > 0)
		{
			ShaderUniform* lastUniform = m_Uniforms[m_Uniforms.size() - 1];
			offset = lastUniform->GetOffset() + lastUniform->GetSize();
			uniform->SetOffset(offset);
		}

		m_Uniforms.push_back(uniform);
	}

	void Shader::PrintUniforms()
	{
		std::vector<ShaderUniform*> uniforms = GetUniforms();

		for (uint i = 0; i < uniforms.size(); i++)
		{
			std::cout << uniforms[i]->StringFromType(uniforms[i]->GetType()) << " " << uniforms[i]->GetName() << " " << uniforms[i]->GetCount() << " " << uniforms[i]->GetSize() << std::endl;
		}
	}

	void Shader::Bind()
	{
		glUseProgram(m_ShaderID);
	}

	void Shader::Unbind()
	{
		glUseProgram(0);
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

	int Shader::GetUniformLocation(const String& name)
	{
		if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
			return m_UniformLocationCache[name];

		int location = glGetUniformLocation(m_ShaderID, name.c_str());

		m_UniformLocationCache[name] = location;
		return location;
	}

	void Shader::SetUniform1i(const String& name, int value)
	{
		glUniform1i(GetUniformLocation(name), value);
	}

	void Shader::SetUniform1iv(const String& name, int* value, int count)
	{
		glUniform1iv(GetUniformLocation(name), count, value);
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

	void Shader::SetUniformVec4(const String& name, const glm::vec4& vec)
	{
		SetUniform4f(name, vec.x, vec.y, vec.z, vec.w);
	}

	void Shader::SetUniform4f(const String& name, float x, float y, float z, float w)
	{
		int test = GetUniformLocation(name);
		glUniform4f(test, x, y, z, w);
	}

	void Shader::SetUniformMat4(const String& name, const glm::mat4& matrix)
	{
		glUniformMatrix4fv(GetUniformLocation(name), 1, GL_TRUE, glm::value_ptr(matrix[0]));
	}

}