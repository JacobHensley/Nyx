#pragma once
#include "Nyx/Common.h"
#include "Nyx/graphics/renderer/API/BufferLayout.h"

namespace Nyx {

	class VertexBuffer
	{
	public:
		VertexBuffer(uint count);
		VertexBuffer(float* data, int count);
		~VertexBuffer();

		inline void SetLayout(const BufferLayout& layout) { m_Layout = layout; }
		inline const BufferLayout& GetLayout() const { return m_Layout; }

		template<typename T = void>
		inline T* Map()
		{
			return (T*)MapInternal();
		}

		void Unmap();

		void Bind() const;
		void Unbind() const;

		inline uint GetBufferID() const { return m_BufferID; }
	private:
		void* MapInternal();

		uint m_BufferID;
		BufferLayout m_Layout;
	};

}