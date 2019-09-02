#include "NXpch.h"
#include "BufferLayout.h"

namespace Nyx {

	BufferLayout::BufferLayout()
	{
	}

	void BufferLayout::CalculateOffsetsAndStride()
	{
		uint32_t offset = 0;
		m_Stride = 0;
		for (auto& element : m_Elements)
		{
			element.offset = offset;
			offset += element.size;
			m_Stride += element.size;
		}
	}

}
