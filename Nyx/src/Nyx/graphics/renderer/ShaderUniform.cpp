#include "NXpch.h"
#include "ShaderUniform.h"
#include "glad/glad.h"

namespace Nyx  {

	ShaderUniform::ShaderUniform(const String& name, UniformType type, uint size, uint offset, RendererID rendererID)
		: m_Name(name), m_Type(type), m_Size(size), m_Offset(offset), m_RendererID(rendererID)
	{
	}

	UniformType ShaderUniform::SpirvTypeToUniformType(spirv_cross::SPIRType type)
	{
		if (type.basetype == spirv_cross::SPIRType::Boolean)
		{
			return UniformType::BOOL;
		}
		else if (type.basetype == spirv_cross::SPIRType::Int)
		{
			return UniformType::INT;
		}
		else if (type.basetype == spirv_cross::SPIRType::Float)
		{
			return UniformType::FLOAT;
		}
		else if (type.basetype == spirv_cross::SPIRType::Struct)
		{
			return UniformType::STRUCT;
		}
		else if (type.basetype == spirv_cross::SPIRType::SampledImage)
		{
			return UniformType::IMAGE;
		}

		return UniformType::NONE;
	}

	const String& ShaderUniform::UniformTypeToString(UniformType type)
	{
		if (type == UniformType::BOOL)
		{
			return "Boolean";
		}
		else if (type == UniformType::INT)
		{
			return "Int";
		}
		else if (type == UniformType::FLOAT)
		{
			return "Float";
		}
		else if (type == UniformType::STRUCT)
		{
			return "Struct";
		}
		else if (type == UniformType::IMAGE)
		{
			return "Image";
		}

		return "None";
	}
}