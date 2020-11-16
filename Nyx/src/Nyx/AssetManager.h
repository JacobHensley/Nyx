#pragma once

#include "Asset.h"

#include "Nyx/graphics/renderer/Mesh.h"
#include "Nyx/graphics/API/Texture.h"
#include "Nyx/graphics/API/TextureCube.h"

namespace Nyx {

	class AssetLoader;

	class AssetManager
	{
		inline static std::unordered_map<UUID, Ref<Asset>, UUIDHash> m_Assets;
		inline static std::unordered_map<UUID, String, UUIDHash> m_AssetPaths;

		friend struct AssetHandle;
		friend class AssetLoader;

	public:
		static const std::unordered_map<UUID, Ref<Asset>, UUIDHash>& GetAssets()
		{
			return m_Assets;
		}

		static const std::unordered_map<UUID, String, UUIDHash>& GetAssetPaths()
		{
			return m_AssetPaths;
		}

	public:
		static String GetAssetTypeName(AssetType type)
		{
			if (type == AssetType::MESH)
				return "Mesh";
			else if (type == AssetType::TEXTURE)
				return "Texture";
			else if (type == AssetType::TEXTURE_CUBE)
				return "TextureCube";
			else if (type == AssetType::MATERIAL)
				return "Material";
			else if (type == AssetType::SCRIPT)
				return "Script";

			else
				return "invaild";
		}

		template<typename T>
		static AssetHandle Load(const std::string& path)
		{
			UUID uuid; // Generate a new UUID for the asset
			return InsertAndLoad<T>(uuid, path);
		}

		static AssetHandle Insert(Ref<Asset> asset, const String& path = "", UUID uuid = UUID())
		{
			m_Assets[uuid] = asset;
			if (path != "")
				m_AssetPaths[uuid] = path;
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
			Ref<Asset> asset = CreateRef<Mesh>(filepath);
			asset->m_AssetType = AssetType::MESH;
			return asset;
		}

		template<>
		static Ref<Asset> LoadAsset<Texture>(const std::string& filepath)
		{
			// TODO: Better
			Ref<Asset> asset = CreateRef<Texture>(filepath);
			asset->m_AssetType = AssetType::TEXTURE;
			return asset;
		}

		template<>
		static Ref<Asset> LoadAsset<TextureCube>(const std::string& filepath)
		{
			// TODO: Better
			Ref<Asset> asset = CreateRef<TextureCube>(filepath);
			asset->m_AssetType = AssetType::TEXTURE_CUBE;
			return asset;
		}

		// Deserialization function
		template<typename T>
		static AssetHandle InsertAndLoad(UUID uuid, const std::string& path)
		{
			// Loading
			NX_CORE_ASSERT(m_Assets.find(uuid) == m_Assets.end(), "Asset UUID already exists in map!");

			Insert(LoadAsset<T>(path), path, uuid);
			return uuid;
		}

		template<typename T>
		static AssetHandle SwapAsset(UUID uuid, const std::string& path)
		{
			// Loading
			NX_CORE_ASSERT(m_Assets.find(uuid) != m_Assets.end(), "Asset UUID does not exist in map!");

			Insert(LoadAsset<T>(path), path, uuid);
			return uuid;
		}

		static Ref<Asset> Get(AssetHandle handle)
		{
			NX_CORE_ASSERT(m_Assets.find(handle) != m_Assets.end(), "Asset not found!");
			return m_Assets.at(handle);
		}

		static Ref<Asset> GetByUUID(UUID id)
		{
			return m_Assets.at(id);
		}

		template<typename T>
		static Ref<T> GetByUUID(UUID id)
		{
			return std::dynamic_pointer_cast<T>(m_Assets.at(id));
		}

	};


}