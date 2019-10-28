#pragma once
#include "Nyx/graphics/renderer/Mesh.h"

namespace Nyx {

	class MeshFactory 
	{
	public:
		static Mesh* GenQuad(float x, float y, float z, float width, float height);
	private:
		static MeshFactory* s_Instance;
	};

}