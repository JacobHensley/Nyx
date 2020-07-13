#include "NXpch.h"
#include "MaterialComponent.h"

namespace Nyx {

	MaterialComponent::MaterialComponent(AssetHandle Material)
		: m_Material(Material)
	{
		NX_CORE_DEBUG(m_Material.GetUUID().GetUUID());
	}

}