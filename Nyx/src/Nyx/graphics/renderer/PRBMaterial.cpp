#include "NXpch.h"
#include "PRBMaterial.h"

namespace Nyx {

	PBRMaterial::PBRMaterial(Shader* shader)
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

	void PBRMaterial::SetAlbedo(glm::vec3 albedoValue)
	{
		SetUniform("u_AlbedoValue", albedoValue);
	}

	void PBRMaterial::SetMetalness(float metalnessValue)
	{
		SetUniform("u_MetalnessValue", metalnessValue);
	}

	void PBRMaterial::SetRoughness(float roughnessValue)
	{
		SetUniform("u_RoughnessValue", roughnessValue);
	}

	void PBRMaterial::SetAlbedoMap(Texture* albedoMap)
	{
		SetTexture("u_AlbedoMap", albedoMap);
	}

	void PBRMaterial::SetMetalnessMap(Texture* metalnessMap)
	{
		SetTexture("u_MetalnessMap", metalnessMap);
	}

	void PBRMaterial::SetRoughnessMap(Texture* roughnessMap)
	{
		SetTexture("u_RoughnessMap", roughnessMap);
	}

	void PBRMaterial::SetNormalMap(Texture* noramlMap)
	{
		SetTexture("u_NormalMap", noramlMap);
	}

	void PBRMaterial::UsingAlbedoMap(bool value)
	{
		SetUniform("u_UsingAlbedoMap", value ? true : false);
	}

	void PBRMaterial::UsingMetalnessMap(bool value)
	{
		SetUniform("u_UsingMetalnessMap", value ? true : false);
	}

	void PBRMaterial::UsingRoughnessMap(bool value)
	{
		SetUniform("u_UsingRoughnessMap", value ? true : false);
	}

	void PBRMaterial::UsingNormalMap(bool value)
	{
		SetUniform("u_UsingNormalMap", value ? true : false);
	}

}