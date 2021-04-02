#include "NXpch.h"
#include "PBRMaterial.h"

namespace Nyx {

	PBRMaterial::PBRMaterial(Ref<Shader> shader, uint32_t materialSortKey, bool opaque)
		: Material(shader)
	{
		SetAlbedo(glm::vec3(1.0f, 1.0f, 1.0f));
		SetMetalness(0.0f);
		SetRoughness(0.5f);

		//Super temp to get rid of warnings
		Ref<Texture> test = CreateRef<Texture>(1, 1);
		test->SetData(new byte[4], 4);
		SetAlbedoMap(test);
		SetMetalnessMap(test);
		SetRoughnessMap(test);
		SetNormalMap(test);

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
		Set("AlbedoValue", albedoValue);
	}

	void PBRMaterial::SetMetalness(float metalnessValue)
	{
		Set("MetalnessValue", metalnessValue);
	}

	void PBRMaterial::SetRoughness(float roughnessValue)
	{
		Set("RoughnessValue", roughnessValue);
	}

	void PBRMaterial::SetAlbedoMap(Ref<Texture> albedoMap)
	{
		SetTexture("m_AlbedoMap", albedoMap);
	}

	void PBRMaterial::SetMetalnessMap(Ref<Texture> metalnessMap)
	{
		SetTexture("m_MetalnessMap", metalnessMap);
	}

	void PBRMaterial::SetRoughnessMap(Ref<Texture> roughnessMap)
	{
		SetTexture("m_RoughnessMap", roughnessMap);
	}

	void PBRMaterial::SetNormalMap(Ref<Texture> noramlMap)
	{
		SetTexture("m_NormalMap", noramlMap);
	}

	void PBRMaterial::UsingAlbedoMap(bool value)
	{
		Set("UsingAlbedoMap", value ? true : false);
	}

	void PBRMaterial::UsingMetalnessMap(bool value)
	{
		Set("UsingMetalnessMap", value ? true : false);
	}

	void PBRMaterial::UsingRoughnessMap(bool value)
	{
		Set("UsingRoughnessMap", value ? true : false);
	}

	void PBRMaterial::UsingNormalMap(bool value)
	{
		Set("UsingNormalMap", value ? true : false);
	}

}