#include "NXpch.h"
#include "PBRMaterial.h"

namespace Nyx {

	PBRMaterial::PBRMaterial(Ref<Shader> shader, uint32_t materialSortKey, bool opaque)
		: Material(shader)
	{
		SetAlbedo(glm::vec3(1.0f, 1.0f, 1.0f));
		SetMetalness(0.0f);
		SetRoughness(0.5f);



		UsingAlbedoMap(false);
		UsingMetalnessMap(false);
		UsingRoughnessMap(false);
		UsingNormalMap(false);
	}

	PBRMaterial::~PBRMaterial()
	{
	}

	void PBRMaterial::SetAlbedo(glm::vec3 albedoValue, Ref<Texture> albedoMap, bool usingMap)
	{
		SetAlbedo(albedoValue);
		SetAlbedoMap(albedoMap);
		UsingAlbedoMap(usingMap);
	}

	void PBRMaterial::SetMetalness(float metalnessValue, Ref<Texture> metalnessMap, bool usingMap)
	{
		SetMetalness(metalnessValue);
		SetMetalnessMap(metalnessMap);
		UsingMetalnessMap(usingMap);
	}

	void PBRMaterial::SetRoughness(float roughnessValue, Ref<Texture> roughnessMap, bool usingMap)
	{
		SetRoughness(roughnessValue);
		SetRoughnessMap(roughnessMap);
		UsingRoughnessMap(usingMap);
	}

	void PBRMaterial::SetNormal(Ref<Texture> normalMap, bool usingMap)
	{
		SetNormalMap(normalMap);
		UsingNormalMap(usingMap);
	}

	void PBRMaterial::SetAlbedo(glm::vec3 albedoValue)
	{
		Set("u_AlbedoValue", albedoValue);
	}

	void PBRMaterial::SetMetalness(float metalnessValue)
	{
		Set("u_MetalnessValue", metalnessValue);
	}

	void PBRMaterial::SetRoughness(float roughnessValue)
	{
		Set("u_RoughnessValue", roughnessValue);
	}

	void PBRMaterial::SetAlbedoMap(Ref<Texture> albedoMap)
	{
		SetTexture("u_AlbedoMap", albedoMap);
	}

	void PBRMaterial::SetMetalnessMap(Ref<Texture> metalnessMap)
	{
		SetTexture("u_MetalnessMap", metalnessMap);
	}

	void PBRMaterial::SetRoughnessMap(Ref<Texture> roughnessMap)
	{
		SetTexture("u_RoughnessMap", roughnessMap);
	}

	void PBRMaterial::SetNormalMap(Ref<Texture> noramlMap)
	{
		SetTexture("u_NormalMap", noramlMap);
	}

	void PBRMaterial::UsingAlbedoMap(bool value)
	{
		Set("u_UsingAlbedoMap", value ? true : false);
	}

	void PBRMaterial::UsingMetalnessMap(bool value)
	{
		Set("u_UsingMetalnessMap", value ? true : false);
	}

	void PBRMaterial::UsingRoughnessMap(bool value)
	{
		Set("u_UsingRoughnessMap", value ? true : false);
	}

	void PBRMaterial::UsingNormalMap(bool value)
	{
		Set("u_UsingNormalMap", value ? true : false);
	}

}