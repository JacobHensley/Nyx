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

		ComplieShaders(shaderSrc);

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

	const TBuiltInResource DefaultTBuiltInResource = { NULL };

	int Shader::CompileShader(uint shader, const String& shaderSrc)
	{
		const char* shaderString = shaderSrc.c_str();

		glslang::InitializeProcess();
		EShLanguage ShaderType = EShLangVertex;
		glslang::TShader Shader(ShaderType);

		const char* glslString = shaderSrc.c_str();

		Shader.setStrings(&glslString, 1);

		int ClientInputSemanticsVersion = 100; // maps to, say, #define VULKAN 100
		glslang::EShTargetClientVersion VulkanClientVersion = glslang::EShTargetOpenGL_450;
		glslang::EShTargetLanguageVersion TargetVersion = glslang::EShTargetSpv_1_0;

		Shader.setEnvInput(glslang::EShSourceGlsl, ShaderType, glslang::EShClientOpenGL, ClientInputSemanticsVersion);
		Shader.setEnvClient(glslang::EShClientOpenGL, VulkanClientVersion);
		Shader.setEnvTarget(glslang::EShTargetSpv, TargetVersion);

		DirStackFileIncluder Includer;

		TBuiltInResource Resources;
		Resources = DefaultTBuiltInResource;
		EShMessages messages = (EShMessages)(EShMsgDefault);

		//Get Path of File
		std::string Path = m_FilePath;
		Includer.pushExternalLocalDirectory(Path);
		const int DefaultVersion = 100;

		std::string PreprocessedGLSL;

		if (!Shader.preprocess(&Resources, DefaultVersion, ENoProfile, false, false, messages, &PreprocessedGLSL, Includer))
		{
			std::cout << "GLSL Preprocessing Failed for: " << m_FilePath << std::endl;
			std::cout << Shader.getInfoLog() << std::endl;
			std::cout << Shader.getInfoDebugLog() << std::endl;
		}

		const char* PreprocessedCStr = PreprocessedGLSL.c_str();
		Shader.setStrings(&PreprocessedCStr, 1);

		if (!Shader.parse(&Resources, 100, false, messages))
		{
			std::cout << "GLSL Parsing Failed for: " << m_FilePath << std::endl;
			std::cout << Shader.getInfoLog() << std::endl;
			std::cout << Shader.getInfoDebugLog() << std::endl;
		}

		glslang::TProgram Program;
		Program.addShader(&Shader);

		if (!Program.link(messages))
		{
			std::cout << "GLSL Linking Failed for: " << m_FilePath << std::endl;
			std::cout << Shader.getInfoLog() << std::endl;
			std::cout << Shader.getInfoDebugLog() << std::endl;
		}

		std::vector<unsigned int> SpirV;
		spv::SpvBuildLogger logger;
		glslang::SpvOptions spvOptions;
		glslang::GlslangToSpv(*Program.getIntermediate(ShaderType), SpirV, &logger, &spvOptions);

		glslang::TReflection reflection(EShReflectionOptions::EShReflectionDefault, EShLangVertex, EShLangFragment);
		reflection.addStage(EShLangVertex, *Program.getIntermediate(EShLangVertex));
		reflection.addStage(EShLangFragment, *Program.getIntermediate(EShLangFragment));

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

}