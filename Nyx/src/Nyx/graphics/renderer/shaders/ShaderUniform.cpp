#include "NXpch.h"
#include "ShaderUniform.h"
#include "glad/glad.h"

namespace Nyx  {

	ShaderUniform::ShaderUniform(const String & name, const String & type, uint count, int sampler)
		: m_Name(name), m_Count(count), m_Offset(0), m_Sampler(sampler) {
		m_Type = StringToType(type);
		m_Size = SizeFromType(m_Type) * m_Count;
	}

	ShaderUniform::ShaderUniform()
	{
	}

	ShaderUniform::~ShaderUniform()
	{
	}

	uint ShaderUniform::SizeFromType(Type type)
	{
		if (type == Type::SHADER_INT) return 4;
		if (type == Type::SHADER_FLOAT) return 4;
		if (type == Type::SHADER_SAMPLER2D) return 4;
		if (type == Type::SHADER_MAT4) return 4 * 4 * 4;
		if (type == Type::SHADER_VEC2) return 4 * 2;
		if (type == Type::SHADER_VEC3) return 4 * 3;
		if (type == Type::SHADER_VEC4) return 4 * 4;
		if (type == Type::SHADER_BOOL) return 1;
		if (type == Type::SHADER_SAMPLERCUBE) return 4;

	//	NX_CORE_ASSERT(false, "Could not find type");
		return NULL;
	}

	Type ShaderUniform::GLToType(int GLType)
	{
		if (GLType == GL_INT) return Type::SHADER_INT;
		if (GLType == GL_FLOAT) return Type::SHADER_FLOAT;
		if (GLType == GL_SAMPLER_2D) return Type::SHADER_SAMPLER2D;
		if (GLType == GL_FLOAT_MAT4) return Type::SHADER_MAT4;
		if (GLType == GL_FLOAT_VEC2) return Type::SHADER_VEC2;
		if (GLType == GL_FLOAT_VEC3) return Type::SHADER_VEC3;
		if (GLType == GL_FLOAT_VEC4) return Type::SHADER_VEC4;
		if (GLType == GL_BOOL) return Type::SHADER_BOOL;
		if (GLType == GL_SAMPLER_CUBE) return Type::SHADER_SAMPLERCUBE;

	//	NX_CORE_ASSERT(false, "Could not find type with name: {0}", name);
		return Type::NONE;
	}

	Type ShaderUniform::StringToType(const String& type)
	{
		if (type == "int") return Type::SHADER_INT;
		if (type == "float") return Type::SHADER_FLOAT;
		if (type == "sampler2D") return Type::SHADER_SAMPLER2D;
		if (type == "mat4") return Type::SHADER_MAT4;
		if (type == "vec2") return Type::SHADER_VEC2;
		if (type == "vec3") return Type::SHADER_VEC3;
		if (type == "vec4") return Type::SHADER_VEC4;
		if (type == "bool") return Type::SHADER_BOOL;
		if (type == "samplerCube") return Type::SHADER_SAMPLERCUBE;
	}

	String ShaderUniform::StringFromType(Type type)
	{
		if (type == Type::SHADER_INT)			return "int";
		if (type == Type::SHADER_FLOAT)			return "float";
		if (type == Type::SHADER_SAMPLER2D)		return "sampler2D";
		if (type == Type::SHADER_MAT4)			return "mat4";
		if (type == Type::SHADER_VEC2)			return "vec2";
		if (type == Type::SHADER_VEC3)			return "vec3";
		if (type == Type::SHADER_VEC4)			return "vec4";
		if (type == Type::SHADER_BOOL)			return "bool";
		if (type == Type::SHADER_SAMPLERCUBE)	return "samplerCube";

	//	NX_CORE_ASSERT(false, "Could not find type");
		return "";
	}

}