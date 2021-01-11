#include "NXpch.h"
#include "Asset.h"
#include "Nyx/Asset/AssetManager.h"

namespace Nyx
{
	Ref<Asset> AssetHandle::GetAssetFromManager(AssetHandle handle)
	{
		return AssetManager::Get(*this);
	}
}