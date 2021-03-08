#pragma once
#include "Material.h"

namespace Nyx {

	class PBRMaterial : public Material
	{
	public:
		PBRMaterial(Ref<Shader> shader, uint32_t materialSortKey = 0, bool opaque = true);
		~PBRMaterial();

		void SetAlbedo(glm::vec3 albedoValue, Ref<Texture> albedoMap, bool usingMap);
		void SetMetalness(float metalnessValue, Ref<Texture> metalnessMap, bool usingMap);
		void SetRoughness(float roughnessValue, Ref<Texture> roughnessMap, bool usingMap);
		void SetNormal(Ref<Texture> normalMap, bool usingMap);

		void SetAlbedo(glm::vec3 albedoValue);
		void SetMetalness(float metalnessValue);
		void SetRoughness(float roughnessValue);

		void SetAlbedoMap(Ref<Texture> albedoMap);
		void SetMetalnessMap(Ref<Texture> metalnessMap);
		void SetRoughnessMap(Ref<Texture> roughnessMap);
		void SetNormalMap(Ref<Texture> noramlMap);

		void UsingAlbedoMap(bool value);
		void UsingMetalnessMap(bool value);
		void UsingRoughnessMap(bool value);
		void UsingNormalMap(bool value);

	};
}