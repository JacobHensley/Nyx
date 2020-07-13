#include "NXpch.h"
#include "Asset.h"

namespace Nyx {

	std::unordered_map<UUID, Ref<Asset>, UUIDHash> AssetManager::m_Assets;

	AssetHandle::AssetHandle(Ref<Asset> asset)
	{
		Set(asset);
	}

}