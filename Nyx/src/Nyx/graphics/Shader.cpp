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
		CompileShaders(m_ShaderSrc);

	}

	//check new calls
	//create buffer class
	uint Shader::CompileShaders(const std::unordered_map<ShaderType, std::string>& shaderSrc) // take by ref
	{
		shaderc::Compiler compiler;
		shaderc::CompileOptions options;

		const int shaderVersion = 430;

		options.SetTargetEnvironment(shaderc_target_env_opengl, shaderVersion);

		GLuint program = glCreateProgram();
		m_ShaderID = program;

		std::vector<GLuint> shaderIDs;
		shaderIDs.reserve(shaderSrc.size());

		uint32_t shaderDataIndex = 0;
		std::vector<uint32_t> shaderData[2];

		for (auto const& shader : shaderSrc)
		{
			ShaderType type = shader.first;
			const std::string& src = shader.second;

			auto compilationResult = compiler.CompileGlslToSpv(src, ShaderTypeToShaderc(type), m_Path.c_str(), options);
			if (compilationResult.GetCompilationStatus() != shaderc_compilation_status_success)
				NX_CORE_ERROR("Warnings ({0}), Errors ({1}) \n{2}", compilationResult.GetNumWarnings(), compilationResult.GetNumErrors(), compilationResult.GetErrorMessage());

			const uint8_t* data = reinterpret_cast<const uint8_t*>(compilationResult.cbegin());
			const uint8_t* dataEnd = reinterpret_cast<const uint8_t*>(compilationResult.cend());
			uint32_t size = dataEnd - data;
			shaderData[shaderDataIndex++] = std::vector<uint32_t>(compilationResult.cbegin(), compilationResult.cend());

			GLuint shader = glCreateShader(ShaderTypeToGL(type));
			glShaderBinary(1, &shader, GL_SHADER_BINARY_FORMAT_SPIR_V, data, size);
			glSpecializeShader(shader, (const GLchar*)"main", 0, nullptr, nullptr);

			shaderIDs.push_back(shader);

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

			for (GLuint shader : shaderIDs)
			{
				glDeleteShader(shader);
			}

			return -1;
		}

		for (GLuint shader : shaderIDs)
		{
			glDetachShader(program, shader);
		}

		for (uint32_t i = 0; i < 2; i++)
		{
			SpirvReflect(shaderData[i]);
		}


		return program;
	}

	void Shader::SpirvReflect(std::vector<uint32_t>& data)
	{
		spirv_cross::Compiler comp(data);
		spirv_cross::ShaderResources res = comp.get_shader_resources();

		uint32_t bufferIndex = 0;
		for (const spirv_cross::Resource& resource : res.uniform_buffers)
		{
			auto& bufferType = comp.get_type(resource.base_type_id);
			int memberCount = bufferType.member_types.size();

			UniformBuffer& buffer = m_UniformBuffers.emplace_back();
			buffer.name = resource.name;
			buffer.bindingPoint = comp.get_decoration(resource.id, spv::DecorationBinding);
			buffer.size = comp.get_declared_struct_size(bufferType);
			buffer.index = m_UniformBuffers.size() - 1;

			buffer.uniforms.reserve(memberCount);

			for (int i = 0; i < memberCount; i++)
			{
				auto& type = comp.get_type(bufferType.member_types[i]);
				int size = comp.get_declared_struct_member_size(bufferType, i);
				int offset = comp.type_struct_member_offset(bufferType, i);
				const std::string& name = comp.get_member_name(bufferType.self, i);

				UniformType uniformType = ShaderUniform::SpirvTypeToUniformType(type);
				RendererID rendererID = GetRendererUniformID(name);

				buffer.uniforms.emplace_back(CreateRef<ShaderUniform>(name, uniformType, size, offset, rendererID));
			}

			glUseProgram(m_ShaderID);
			glCreateBuffers(1, &buffer.openGLID);
			glBindBuffer(GL_UNIFORM_BUFFER, buffer.openGLID);
			glBufferData(GL_UNIFORM_BUFFER, buffer.size, nullptr, GL_DYNAMIC_DRAW);
			glBindBufferBase(GL_UNIFORM_BUFFER, buffer.bindingPoint, buffer.openGLID);
		}

		glUseProgram(m_ShaderID);
		int32_t sampler = 0;
		for (const spirv_cross::Resource& resource : res.sampled_images)
		{
			auto& type = comp.get_type(resource.base_type_id);
			auto location = comp.get_decoration(resource.id, spv::DecorationLocation);
			const std::string& name = resource.name;
			uint dimension = type.image.dim;

			RendererID rendererID = GetRendererUniformID(name);
			Ref<ShaderResource> resource = CreateRef<ShaderResource>(name, dimension, location, rendererID, sampler);
			glUniform1i(location, sampler);

			if (rendererID != RendererID::NONE)
				m_Resources[UniformSystemType::RENDERER].push_back(resource);
			else
				m_Resources[UniformSystemType::MATERIAL].push_back(resource);
			
			sampler++;
		}
	}

	RendererID Shader::GetRendererUniformID(const std::string& name)
	{
		if (name == "ModelMatrix")
		{
			return RendererID::MODEL_MATRIX;
		}
		else if (name == "ViewMatrix")
		{
			return RendererID::VIEW_MATRIX;
		}
		else if (name == "ProjMatrix")
		{
			return RendererID::PROJ_MATRIX;
		}	
		else if (name == "InverseVP")
		{
			return RendererID::INVERSE_VP;
		}
		else if (name == "MVP")
		{
			return RendererID::MVP;
		}
		else if (name == "CameraPosition")
		{
			return RendererID::CAMERA_POSITION;
		}
		else if (name == "r_BRDFLutTexture")
		{
			return RendererID::BRDF_LUT;
		}	
		else if (name == "r_IrradianceTexture")
		{
			return RendererID::IRRADIANCE_TEXTURE;
		}	
		else if (name == "r_RadianceTexture")
		{
			return RendererID::RADIANCE_TEXTURE;
		}
		else if (name == "DirectionLight")
		{
			return RendererID::DIRECTIONAL_LIGHT;
		}
		else if (name == "PLight")
		{
			return RendererID::POINT_LIGHT;
		}

		return RendererID::NONE;
	}

	std::unordered_map<ShaderType, std::string> Shader::SplitShaders(const std::string& path)
	{
		std::ifstream stream(path);

		std::unordered_map<ShaderType, std::string> result;

		ShaderType type = ShaderType::NONE;
		std::stringstream ss[4];
		std::string line;

		int lineNumber = 1;

		while (getline(stream, line))
		{
			if (line.find("#Shader") != std::string::npos)
			{
				if (line.find("Vertex") != std::string::npos)
				{
					type = ShaderType::VERTEX;
				}
				else if (line.find("Fragment") != std::string::npos)
				{
					type = ShaderType::FRAGMENT;
				}
				else if (line.find("Compute") != std::string::npos)
				{
					type = ShaderType::COMPUTE;
				}
				else if (line.find("Geometry") != std::string::npos)
				{
					type = ShaderType::GEOMETRY;
				}
			}
			else
			{
				result[type] += line + '\n';

				if (line.find("#version") != std::string::npos)
				{
					ss[(int)type] << "#line " + std::to_string(lineNumber + 3) << '\n';
				}

				lineNumber++;
			}
		}

		return result;
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
	}

	void Shader::UploadUniformBuffer(uint index, byte* buffer, uint size) //asserts 
	{
		UniformBuffer& ub = m_UniformBuffers[index];
		glBindBuffer(GL_UNIFORM_BUFFER, ub.openGLID);
		glBindBufferBase(GL_UNIFORM_BUFFER, ub.bindingPoint, ub.openGLID);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, size, (const void*)buffer);
	}

	//store as array of vector
	std::vector<UniformBuffer*> Shader::GetUniformBuffers(UniformSystemType type) //return const ref
	{
		char identifier = 0;
		if (type == UniformSystemType::MATERIAL)
			identifier = 'm';
		else if (type == UniformSystemType::RENDERER)
			identifier = 'r';

		std::vector<UniformBuffer*> result;

		for (int i = 0; i < m_UniformBuffers.size(); i++)
		{
			const std::string& name = m_UniformBuffers[i].name;
			if (name.front() == identifier)
				result.push_back(&m_UniformBuffers[i]);
		}

		return result;
	}

	const std::vector<Ref<ShaderResource>>& Shader::GetResources(UniformSystemType type)
	{
		return m_Resources[type];
	}

	Ref<ShaderResource> Shader::FindShaderResource(const std::string& name, UniformSystemType type)
	{
		if (m_Resources.find(type) == m_Resources.end())
			return nullptr;

		for (Ref<ShaderResource> resource : m_Resources.at(type))
		{
			if (resource->GetName() == name)
				return resource;
		}
		return nullptr;
	}

	shaderc_shader_kind Shader::ShaderTypeToShaderc(ShaderType type)
	{
		if (type == ShaderType::VERTEX)
		{
			return shaderc_vertex_shader;
		}
		else if (type == ShaderType::FRAGMENT)
		{
			return shaderc_fragment_shader;
		}
		else if (type == ShaderType::COMPUTE)
		{
			return shaderc_compute_shader;
		}
		else if (type == ShaderType::GEOMETRY)
		{
			return shaderc_geometry_shader;
		}
	}

	GLenum Shader::ShaderTypeToGL(ShaderType type)
	{
		if (type == ShaderType::VERTEX)
		{
			return GL_VERTEX_SHADER;
		}
		else if (type == ShaderType::FRAGMENT)
		{
			return GL_FRAGMENT_SHADER;
		}
		else if (type == ShaderType::COMPUTE)
		{
			return GL_COMPUTE_SHADER;
		}
		else if (type == ShaderType::GEOMETRY)
		{
			return GL_GEOMETRY_SHADER;
		}
	}

	const std::string& Shader::ShaderTypeToString(ShaderType type)
	{
		if (type == ShaderType::VERTEX)
		{
			return "Vertex";
		}
		else if (type == ShaderType::FRAGMENT)
		{
			return "Fragment";
		}
		else if (type == ShaderType::COMPUTE)
		{
			return "Compute";
		}
		else if (type == ShaderType::GEOMETRY)
		{
			return "Geometry";
		}
	}

	void Shader::SetUniform1i(const std::string& name, int value)
	{
		glUniform1i(glGetUniformLocation(m_ShaderID, name.c_str()), value);
	}

	void Shader::SetUniform1f(const std::string& name, float value)
	{
		glUniform1f(glGetUniformLocation(m_ShaderID, name.c_str()), value);
	}

	void Shader::SetUniform2f(const std::string& name, const glm::vec2& vec)
	{
		glUniform2f(glGetUniformLocation(m_ShaderID, name.c_str()), vec.x, vec.y);
	}

	void Shader::SetUniform3f(const std::string& name, const glm::vec3& vec)
	{
	}

	void Shader::SetUniform4f(const std::string& name, const glm::vec4& vec)
	{
	}

	void Shader::SetUniformMat4(const std::string& name, const glm::mat4& matrix)
	{
	}

}