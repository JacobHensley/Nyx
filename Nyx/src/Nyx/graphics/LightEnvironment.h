#pragma once
#include <glm/glm.hpp>

namespace Nyx {

	struct DirectionalLight
	{
		glm::vec3 Radiance = { 1.0f, 1.0f, 1.0f };
		float padding01 = 0.0f;
		glm::vec3 Direction = { 1.0f, 1.0f, 1.0f };
		uint32_t Active = false;

		DirectionalLight() = default;
		DirectionalLight(glm::vec3 radiance, glm::vec3 direction)
			: Radiance(radiance), Direction(direction)
		{
		}
	};

	struct PointLight
	{
		glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
		float padding03 = 0.0f;
		glm::vec3 Radiance = { 1.0f, 1.0f, 1.0f };
		float Intensity = 1.0f;
		uint32_t Active = false;
		float p[3];

		PointLight() = default;
		PointLight(glm::vec3 position, glm::vec3 radiance, float intensity)
			: Position(position), Radiance(radiance), Intensity(intensity)
		{
		}
	};

	class LightEnvironment 
	{
	public:
		LightEnvironment() = default;
		~LightEnvironment() = default;

		inline void AddDirectionalLight(Ref<DirectionalLight> light) { m_DirectionalLights.push_back(light); }
		inline void AddPointLight(Ref<PointLight> light) { m_PointLights.push_back(light); }

		inline void Clear() { m_DirectionalLights.clear(); m_PointLights.clear(); }

		inline std::vector<Ref<DirectionalLight>>& GetDirectionalLights() { return m_DirectionalLights; }
		inline std::vector<Ref<PointLight>>& GetPointLights() { return m_PointLights; }

	private:
		std::vector<Ref<DirectionalLight>> m_DirectionalLights;
		std::vector<Ref<PointLight>> m_PointLights;
	};

	struct EnvironmentMap
	{
		EnvironmentMap() = default;
		EnvironmentMap(AssetHandle radianceMap, AssetHandle irradianceMap)
			: radianceMap(radianceMap), irradianceMap(irradianceMap)
		{
		}

		AssetHandle radianceMap;
		AssetHandle irradianceMap;
	};
}