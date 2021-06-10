#pragma once
#include "Nyx/Core/Core.h"
#include "Nyx/Asset/UUID.h"

namespace Nyx {

	enum class AssetType
	{
		INVAILD = 0, MESH, TEXTURE, TEXTURE_CUBE, SCRIPT, MATERIAL
	};

    class Asset
    {
    public:
		virtual ~Asset() = default;

		virtual bool Reload() { return false; }

		bool operator==(const Asset& other) const
		{
			return false;
		}

		AssetType m_AssetType;
	};

	struct AssetHandle
	{
		AssetHandle(UUID uuid)
			: m_UUID(uuid) { }

		AssetHandle()
			: m_UUID({ 0 })
		{
		}

		UUID GetUUID() const { return m_UUID; }

		template<typename T>
		Ref<T> Get() { NX_CORE_ASSERT(IsValid(), "Invalid asset handle!"); return std::dynamic_pointer_cast<T>(GetAssetFromManager(*this)); }

		operator UUID () const { return m_UUID; }

		bool IsValid() const { return m_UUID != 0; }
		operator bool() const { return IsValid(); }

	private:
		Ref<Asset> GetAssetFromManager(AssetHandle handle);

	private:
		UUID m_UUID;
	};

}