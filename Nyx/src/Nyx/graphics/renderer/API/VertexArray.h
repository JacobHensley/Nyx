#pragma once
#include "VertexBuffer.h"

namespace Nyx {

	class VertexArray
	{
	public:
		VertexArray();
		~VertexArray();

		void PushVertexBuffer(VertexBuffer* VB);

		void Bind();
		void Unbind();
	private:
		uint m_VertexArray;
	};

}