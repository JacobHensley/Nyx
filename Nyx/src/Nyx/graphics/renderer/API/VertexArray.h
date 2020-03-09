#pragma once
#include "VertexBuffer.h"
#include "BufferLayout.h"

namespace Nyx {

	#define GL_BYTE 0x1400
	#define GL_UNSIGNED_BYTE 0x1401
	#define GL_SHORT 0x1402
	#define GL_UNSIGNED_SHORT 0x1403
	#define GL_INT 0x1404
	#define GL_UNSIGNED_INT 0x1405
	#define GL_FLOAT 0x1406
	#define GL_BOOL 0x8B56

	static uint ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
			case ShaderDataType::Float:  return GL_FLOAT;
			case ShaderDataType::Vec2:   return GL_FLOAT;
			case ShaderDataType::Vec3:   return GL_FLOAT;
			case ShaderDataType::Vec4:   return GL_FLOAT;
			case ShaderDataType::Mat3:   return GL_FLOAT;
			case ShaderDataType::Mat4:   return GL_FLOAT;
			case ShaderDataType::Int:    return GL_INT;
			case ShaderDataType::Vec2i:	 return GL_INT;
			case ShaderDataType::Vec3i:  return GL_INT;
			case ShaderDataType::Vec4i:  return GL_INT;
			case ShaderDataType::Bool:   return GL_BOOL;
		}

		NX_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	class VertexArray
	{
	public:
		VertexArray();
		~VertexArray();

	public:
		void PushVertexBuffer(Ref<VertexBuffer> vertexBuffer);

		void Bind();
		void Unbind();

	private:
		uint m_VertexArray;
	};

	#undef GL_BYTE
	#undef GL_UNSIGNED_BYTE 
	#undef GL_SHORT 
	#undef GL_UNSIGNED_SHORT 
	#undef GL_INT 
	#undef GL_UNSIGNED_INT 
	#undef GL_FLOAT 
	#undef GL_BOOL

}