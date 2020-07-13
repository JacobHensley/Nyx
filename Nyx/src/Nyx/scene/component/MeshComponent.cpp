#include "NXpch.h"
#include "MeshComponent.h"

namespace Nyx {

	MeshComponent::MeshComponent(AssetHandle mesh)
		: m_Mesh(mesh)
	{
		NX_CORE_DEBUG(m_Mesh.GetUUID().GetUUID());
	}

}