#pragma once
#include "Nyx/Core/Core.h"

namespace Nyx {

	class IndexBuffer
	{
	public:
		IndexBuffer(const uint* data, uint count);
		~IndexBuffer();

	public:
		void Draw(bool depthTesting = true) const;
		void Draw(uint count) const;

		void Bind() const;
		void Unbind() const;

		void SetData(const uint* data, uint count);

		inline uint GetBufferID() const { return m_BufferID; }
		inline uint GetCount() const { return m_Count; }

	private:
		uint m_BufferID;
		uint m_Count;
	};

}
