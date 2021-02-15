#pragma once
#include "Asset.h"
#include "Nyx/Graphics/Mesh.h"
#include "Nyx/Graphics/API/Texture.h"
#include "Nyx/Graphics/API/TextureCube.h"
#include <String>
#include <filesystem>
#include <memory>

namespace Nyx {

	class AssetManager
	{
		inline static std::unordered_map<UUID, Ref<Asset>, UUIDHash> m_Assets;
		inline static std::unordered_map<UUID, std::string, UUIDHash> m_AssetPaths;
		inline static std::unordered_map<std::string, UUID> m_Paths;

		friend struct AssetHandle;
		friend class SceneSerializer;

	public:
		static const std::unordered_map<UUID, Ref<Asset>, UUIDHash>& GetAssets()
		{
			return m_Assets;
		}

		static const std::unordered_map<UUID, std::string, UUIDHash>& GetAssetPaths()
		{
			return m_AssetPaths;
		}
	public:
		static std::string GetAssetTypeName(AssetType type)
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
			std::string absolutePath = GetAbsolutePath(path);
			if (m_Paths.find(absolutePath) != m_Paths.end())
			{
				AssetHandle handle = (m_Paths[absolutePath]);
				NX_CORE_TRACE("Found existing asset [{0}] ({1})", (uint64_t)handle.GetUUID(), path);
				return handle;
			}

			UUID uuid; // Generate a new UUID for the asset
			return InsertAndLoad<T>(uuid, absolutePath);
		}

		static Ref<Asset> Get(AssetHandle handle)
		{
			NX_CORE_ASSERT(m_Assets.find(handle) != m_Assets.end(), "Asset not found!");
			return m_Assets.at(handle);
		}

		template<typename T>
		static Ref<T> Get(AssetHandle handle)
		{
			NX_CORE_ASSERT(m_Assets.find(handle) != m_Assets.end(), "Asset not found!");
			return std::dynamic_pointer_cast<T>(m_Assets.at(handle));
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

		static AssetHandle Insert(Ref<Asset> asset)
		{
			UUID uuid;
			m_Assets[uuid] = asset;
			return uuid;
		}
	private:
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

		static AssetHandle Insert(Ref<Asset> asset, const std::string& path, UUID uuid)
		{
			NX_CORE_ASSERT(!path.empty(), "Path cannot be empty");

			std::string absolutePath = GetAbsolutePath(path);

			m_Assets[uuid] = asset;
			m_AssetPaths[uuid] = absolutePath;
			m_Paths[absolutePath] = uuid;
			return uuid;
		}

		// Deserialization function
		template<typename T>
		static AssetHandle InsertAndLoad(UUID uuid, const std::string& path)
		{
			// Loading
			NX_CORE_ASSERT(m_Assets.find(uuid) == m_Assets.end(), "Asset UUID already exists in map!");

			std::string absolutePath = GetAbsolutePath(path);
			AssetHandle handle;
			if (m_Paths.find(absolutePath) != m_Paths.end())
			{
				handle = AssetHandle(m_Paths[absolutePath]);
				Insert(m_Assets[handle.GetUUID()], path, uuid);
				NX_CORE_TRACE("Found existing asset [{0}], inserting as [{1}] ({2})", (uint64_t)handle.GetUUID(), (uint64_t)uuid, path);
			}
			else
			{
				Insert(LoadAsset<T>(path), path, uuid);
			}
			return uuid;
		}

		template<typename T>
		static AssetHandle TryInsertAndLoad(UUID uuid, const std::string& path)
		{
			if (m_Assets.find(uuid) != m_Assets.end())
				return uuid;
			
			return InsertAndLoad<T>(uuid, path);
		}

		template<typename T>
		static AssetHandle SwapAsset(UUID uuid, const std::string& path)
		{
			// Loading
			NX_CORE_ASSERT(m_Assets.find(uuid) != m_Assets.end(), "Asset UUID does not exist in map!");

			Insert(LoadAsset<T>(path), path, uuid);
			return uuid;
		}

		static std::string GetAbsolutePath(const std::string& path)
		{
			std::filesystem::path absolutePath = std::filesystem::canonical(path);
			if (std::filesystem::exists(absolutePath))
				return absolutePath.string();

			return std::string();
		}

	};


}