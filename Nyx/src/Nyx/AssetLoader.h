#pragma once

#include "Nyx/Common.h"

#include "Nyx/graphics/renderer/Mesh.h"
#include "Nyx/graphics/API/Texture.h"

namespace Nyx {

	class Asset;
	class AssetLoader 
	{
	public:
		template<typename T>
		static Ref<Asset> LoadAsset(const std::string& filepath)
		{
			static_assert(false, "");
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
	};
}