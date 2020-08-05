#pragma once

#include "Asset.h"

#include "Nyx/graphics/renderer/Mesh.h"
#include "Nyx/graphics/API/Texture.h"

namespace Nyx {

	class AssetManager;
	struct AssetHandle
	{
		AssetHandle(UUID uuid)
			: m_UUID(uuid) { }

		AssetHandle()
			: m_UUID({ 0 })
		{
		}

		UUID GetUUID() { return m_UUID; }

		template<typename T>
		Ref<T> Get() { NX_CORE_ASSERT(IsValid(), "Invalid asset handle!"); return std::dynamic_pointer_cast<T>(AssetManager::Get(*this)); }

		operator UUID () const { return m_UUID; }

		bool IsValid() const { return m_UUID != 0; }
		operator bool() const { return IsValid(); }
	private:
		UUID m_UUID;
	};

	class AssetLoader;

	class AssetManager
	{
		inline static std::unordered_map<UUID, Ref<Asset>, UUIDHash> m_Assets;
		inline static std::unordered_map<UUID, String, UUIDHash> m_AssetPaths;

		friend struct AssetHandle;
		friend class AssetLoader;

	public:
		template<typename T>
		static AssetHandle Load(const std::string& path)
		{
			UUID uuid; // Generate a new UUID for the asset
			return InsertAndLoad<T>(uuid, path);
		}

		static AssetHandle Insert(Ref<Asset> asset, UUID uuid = UUID())
		{
			m_Assets[uuid] = asset;
			return uuid;
		}

		template<typename T>
		static Ref<Asset> LoadAsset(const std::string& filepath)
		{
			static_assert(false, "Could not find LoadAsset implementation");
		}

		template<>
		static Ref<Asset> LoadAsset<Mesh>(const std::string& filepath)
		{
			// TODO: Better
			return CreateRef<Mesh>(filepath);
		}

		template<>
		static Ref<Asset> LoadAsset<Texture>(const std::string& filepath)
		{
			// TODO: Better
			return CreateRef<Texture>(filepath);
		}

		// Deserialization function
		template<typename T>
		static AssetHandle InsertAndLoad(UUID uuid, const std::string& path)
		{
			// Loading
			NX_CORE_ASSERT(m_Assets.find(uuid) == m_Assets.end(), "Asset UUID already exists in map!");

			Insert(LoadAsset<T>(path), uuid);
			return uuid;
		}

		static Ref<Asset> Get(AssetHandle handle)
		{
			NX_CORE_ASSERT(m_Assets.find(handle) != m_Assets.end(), "Asset not found!");
			return m_Assets.at(handle);
		}

		template<typename T>
		static Ref<T> GetByUUID(UUID id)
		{
			return std::dynamic_pointer_cast<T>(m_Assets.at(id));
		}

	};


}