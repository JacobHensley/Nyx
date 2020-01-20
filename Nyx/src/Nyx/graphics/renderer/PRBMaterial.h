#pragma once
#include "Material.h"

namespace Nyx {

	class PBRMaterial : public Material
	{
	public:
		PBRMaterial(Shader* shader);
		~PBRMaterial();

		void SetAlbedo(glm::vec3 albedoValue, Texture* albedoMap, bool usingMap);
		void SetMetalness(float metalnessValue, Texture* metalnessMap, bool usingMap);
		void SetRoughness(float roughnessValue, Texture* roughnessMap, bool usingMap);
		void SetNormal(Texture* normalMap, bool usingMap);

		void SetAlbedo(glm::vec3 albedoValue);
		void SetMetalness(float metalnessValue);
		void SetRoughness(float roughnessValue);

		void SetAlbedoMap(Texture* albedoMap);
		void SetMetalnessMap(Texture* metalnessMap);
		void SetRoughnessMap(Texture* roughnessMap);
		void SetNormalMap(Texture* noramlMap);

		void UsingAlbedoMap(bool value);
		void UsingMetalnessMap(bool value);
		void UsingRoughnessMap(bool value);
		void UsingNormalMap(bool value);

	};
}