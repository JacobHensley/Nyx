#pragma once
#include "Nyx/UUID.h"
#include "NXpch.h"

namespace Nyx {


	struct UUIDHash {
		std::size_t operator()(UUID e) const {
			return static_cast<std::size_t>(e);
		}
	};

	class Asset
	{
	public:
		virtual ~Asset()
		{

		}

		bool operator==(const Asset& other) const
		{
			return false;
		}
	};

	class AssetManager
	{
		static std::unordered_map<UUID, Ref<Asset>, UUIDHash> m_Assets;

		friend class AssetHandle;

	public:
		template<typename T> 
		static Ref<T> GetByUUID(UUID id)
		{
			return std::dynamic_pointer_cast<T>(m_Assets.at(id));
		}

	};

	class AssetHandle
	{
	public:
		AssetHandle(Ref<Asset> asset);

		template<typename T>
		void Set(Ref<T> asset)
		{
			AssetManager::m_Assets[id] = asset;
		}

		template<typename T>
		Ref<T> Get()
		{
			return std::dynamic_pointer_cast<T>(AssetManager::GetByUUID<T>(id));
		}

		inline UUID GetUUID() { return id; };

	private:
		UUID id;
	};

}