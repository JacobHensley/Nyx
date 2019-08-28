#pragma once
#include "Nyx/Common.h"
#include "BufferLayout.h"

namespace Nyx {

	class VertexBuffer
	{
	public:
		VertexBuffer();
		VertexBuffer(uint size);
		VertexBuffer(void* data, int size);
		~VertexBuffer();

		void SetData(float* data, int size);

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