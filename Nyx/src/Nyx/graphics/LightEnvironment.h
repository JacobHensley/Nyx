#pragma once
#include <glm/glm.hpp>

namespace Nyx {

	struct DirectionalLight
	{
		glm::vec3 Direction = { 1.0f, 1.0f, 1.0f };
		float padding01 = 0.0f;
		glm::vec3 Radiance = { 1.0f, 1.0f, 1.0f };
		uint32_t Active = false;

		DirectionalLight() = default;
		DirectionalLight(glm::vec3 direction, glm::vec3 radiance)
			: Direction(direction), Radiance(radiance)
		{
		}
	};

	struct PointLight
	{
		glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
		float padding01 = 0.0f;
		glm::vec3 Radiance = { 1.0f, 1.0f, 1.0f };
		float Intensity = 1.0f;
		uint32_t Active = false;
		float padding02[3];

		PointLight() = default;
		PointLight(glm::vec3 position, glm::vec3 radiance, float intensity)
			: Position(position), Radiance(radiance), Intensity(intensity)
		{
		}
	};

	struct SpotLight
	{
		glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
		float padding01 = 0.0f;
		glm::vec3 Direction = { 0.0f, -1.0f, 0.0f };
		float padding02 = 0.0f;
		glm::vec3 Radiance = { 1.0f, 1.0f, 1.0f };
		float InnerCutoff = 25.0f; // Degrees
		float OuterCutoff = 45.0f;
		float Intensity = 1.0f;
		uint32_t Active = false;
		float padding03 = 0.0f;

		SpotLight() = default;
		SpotLight(glm::vec3 position, glm::vec3 direction, glm::vec3 radiance, float innerCutoff, float outerCutoff, float intensity)
			: Position(position), Direction(direction), Radiance(radiance), InnerCutoff(innerCutoff), OuterCutoff(outerCutoff), Intensity(intensity)
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
		inline void AddSpotLight(Ref<SpotLight> light) { m_SpotLights.push_back(light); }

		inline void Clear() { m_DirectionalLights.clear(); m_PointLights.clear(); m_SpotLights.clear(); }

		inline std::vector<Ref<DirectionalLight>>& GetDirectionalLights() { return m_DirectionalLights; }
		inline std::vector<Ref<PointLight>>& GetPointLights() { return m_PointLights; }
		inline std::vector<Ref<SpotLight>>& GetSpotLights() {return m_SpotLights; }

	private:
		std::vector<Ref<DirectionalLight>> m_DirectionalLights;
		std::vector<Ref<PointLight>> m_PointLights;
		std::vector<Ref<SpotLight>> m_SpotLights;
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