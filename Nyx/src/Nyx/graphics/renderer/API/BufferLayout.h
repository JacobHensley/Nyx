#pragma once
#include "NXpch.h"
#include <glm/glm.hpp>

namespace Nyx {

	enum class ShaderDataType
	{
		None = 0, Float, Vec2, Vec3, Vec4, Mat3, Mat4, Int, Vec2i, Vec3i, Vec4i, Bool
	};

	static uint ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
			case ShaderDataType::Float:    return 4;
			case ShaderDataType::Vec2:	   return 4 * 2;
			case ShaderDataType::Vec3:	   return 4 * 3;
			case ShaderDataType::Vec4:	   return 4 * 4;
			case ShaderDataType::Mat3:     return 4 * 3 * 3;
			case ShaderDataType::Mat4:     return 4 * 4 * 4;
			case ShaderDataType::Int:      return 4;
			case ShaderDataType::Vec2i:    return 4 * 2;
			case ShaderDataType::Vec3i:    return 4 * 3;
			case ShaderDataType::Vec4i:    return 4 * 4;
			case ShaderDataType::Bool:     return 1;
		}

		NX_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	struct BufferElement
	{
		String name;
		ShaderDataType type;
		byte count;
		byte size;
		uint offset;

		BufferElement(ShaderDataType type, const std::string& name)
			: name(name), type(type), size(ShaderDataTypeSize(type)), offset(0)
		{
		}

		uint32_t GetComponentCount() const
		{
			switch (type)
			{
			case ShaderDataType::Float: return 1;
			case ShaderDataType::Vec2:  return 2;
			case ShaderDataType::Vec3:  return 3;
			case ShaderDataType::Vec4:  return 4;
			case ShaderDataType::Mat3:  return 3 * 3;
			case ShaderDataType::Mat4:  return 4 * 4;
			case ShaderDataType::Int:   return 1;
			case ShaderDataType::Vec2i: return 2;
			case ShaderDataType::Vec3i: return 3;
			case ShaderDataType::Vec4i: return 4;
			case ShaderDataType::Bool:  return 1;
			}

			NX_CORE_ASSERT(false, "Unknown ShaderDataType!");
			return 0;
		}
	};

	class BufferLayout
	{
	public:
		BufferLayout();

		BufferLayout(const std::initializer_list<BufferElement>& elements)
			: m_Elements(elements), m_Stride(0)
		{
			CalculateOffsetsAndStride();
		}

	public:
		inline const std::vector<BufferElement>& GetElements() const { return m_Elements; }
		inline uint GetStride() const { return m_Stride; }

	private:
		std::vector<BufferElement> m_Elements;
		uint m_Stride;

		void CalculateOffsetsAndStride();
	};

}