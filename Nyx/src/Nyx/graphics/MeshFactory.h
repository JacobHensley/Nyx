#pragma once
#include "Nyx/Core/Core.h"
#include "Nyx/Graphics/Mesh.h"

namespace Nyx {

	class MeshFactory
	{
	public:
		static Ref<Mesh> GenQuad(float x, float y, float z, float width, float height);

	private:
		static MeshFactory* s_Instance;
	};

}