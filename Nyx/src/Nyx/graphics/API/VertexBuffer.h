#pragma once
#include "Nyx/Core/Core.h"
#include "BufferLayout.h"

namespace Nyx {

	class VertexBuffer
	{
	public:
		VertexBuffer(uint size);
		VertexBuffer(void* data, int size);
		~VertexBuffer();

	public:
		template<typename T = void>
		inline T* Map()
		{
			return (T*)MapInternal();
		}

		void Unmap();

		void Bind() const;
		void Unbind() const;

		void SetData(const void* data, uint size);
		inline void SetLayout(const BufferLayout& layout) { m_Layout = layout; }

		inline uint GetBufferID() const { return m_BufferID; }
		inline const BufferLayout& GetLayout() const { return m_Layout; }

	private:
		void* MapInternal();

		uint m_BufferID;
		BufferLayout m_Layout;
		uint m_Size = 0;
	};

}