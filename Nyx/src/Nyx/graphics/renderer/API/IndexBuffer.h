#pragma once
#include "Nyx/Common.h"

namespace Nyx {

	class IndexBuffer
	{
	public:
		IndexBuffer(const uint* data, uint count);
		~IndexBuffer();

		void Draw() const;
		void Draw(uint count) const;

		void Bind() const;
		void Unbind() const;

		inline uint GetBufferID() const { return m_BufferID; }
		inline uint GetCount() const { return m_Count; }
	private:
		uint m_BufferID;
		uint m_Count;
	};

}
