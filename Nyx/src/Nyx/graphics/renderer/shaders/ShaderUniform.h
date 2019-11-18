#pragma once

namespace Nyx {

	enum Type
	{
		NONE = 0, SHADER_INT = 1, SHADER_FLOAT = 2, SHADER_SAMPLER2D = 3, SHADER_MAT4 = 4, SHADER_VEC2 = 5, SHADER_VEC3 = 6, SHADER_VEC4 = 7, SHADER_BOOL = 8, SHADER_SAMPLERCUBE = 9
	};

	class ShaderUniform
	{
	public:
		ShaderUniform(const String& name, const String& type, uint count, int sampler);
		ShaderUniform();
		~ShaderUniform();

		uint SizeFromType(Type type);

		Type GLToType(int GLType);
		Type StringToType(const String& type);
		String StringFromType(Type type);

		inline const String& GetName() { return m_Name; }
		inline const Type GetType() { return m_Type; }
		inline const uint GetCount() { return m_Count; }
		inline const uint GetSize() { return m_Size; }
		inline const uint GetOffset() { return m_Offset; }
		inline const int GetSampler() { return m_Sampler; }

	private:
		inline void SetOffset(int offset) { m_Offset = offset; }

	private:
		friend class Shader;
		const String m_Name;
		Type m_Type = Type::NONE;
		uint m_Count = 0;
		uint m_Size = 0;
		uint m_Offset = 0;
		int m_Sampler = 0;
	};

}