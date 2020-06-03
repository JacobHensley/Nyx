#pragma once
#include "spirv_cross.hpp"

namespace Nyx {

	enum class UniformType
	{
		NONE = -1, BOOL, INT, FLOAT, STRUCT, IMAGE
	};

	enum class RendererID
	{
		NONE = -1, MODEL_MATRIX, VIEW_MATRIX, PROJ_MATRIX, INVERSE_VP, MVP, CAMERA_POSITION, BRDF_LUT, IRRADIANCE_TEXTURE, RADIANCE_TEXTURE
	};

	class ShaderUniform
	{
	public:
		ShaderUniform(const String& name, UniformType type, uint size, uint offset, RendererID rendererID);

		static UniformType SpirvTypeToUniformType(spirv_cross::SPIRType type);
		static const String& UniformTypeToString(UniformType type);

		inline const String& GetName() { return m_Name; }
		inline UniformType GetType() { return m_Type; }
		inline uint GetSize() { return m_Size; }
		inline uint GetOffset() { return m_Offset; }

		inline RendererID GetRendererID() { return m_RendererID; }

	private:
		const String m_Name;
		UniformType m_Type;
		uint m_Size;
		uint m_Offset;

		RendererID m_RendererID;
	};


	class ShaderResource
	{
	public:
		ShaderResource::ShaderResource(const String& name, uint dimension, uint location, RendererID rendererID, int sampler)
			: m_Name(name), m_Dimension(dimension), m_Location(location), m_RendererID(rendererID), m_Sampler(sampler)
		{
		}

		inline const String& GetName() { return m_Name; }
		inline uint GetLocation() { return m_Location; }
		inline uint GetDimension() { return m_Dimension; }
		inline int GetSampler() { return m_Sampler; }
		
		inline RendererID GetRendererID() { return m_RendererID; }

	private:
		const String m_Name;
		uint m_Location;
		uint m_Dimension;
		int m_Sampler;

		RendererID m_RendererID;
	};

}