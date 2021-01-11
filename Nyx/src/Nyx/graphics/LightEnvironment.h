#pragma once
#include "Nyx/Core/Core.h"
#include <glm/glm.hpp>

namespace Nyx {

	struct DirectionalLight
	{
		glm::vec3 radiance;
		glm::vec3 direction;

		float active = true;

		DirectionalLight(glm::vec3 radiance, glm::vec3 direction, bool active = true)
			: radiance(radiance), direction(direction), active(1.0f)
		{
		}

		DirectionalLight()
			: radiance(glm::vec3()), direction(glm::vec3()), active(1.0f)
		{
		}
	};

	struct PointLight
	{
		glm::vec3 position = { 1.0f, 1.0f, 0.0f };
		glm::vec3 radiance = { 1.0f, 1.0f, 1.0f };
		float intensity = 1.0f;
	};

	class LightEnvironment 
	{
	public:
		LightEnvironment();
		~LightEnvironment();

		inline void SetDirectionalLight(Ref<DirectionalLight>& light) { m_DirectionalLight = light; }
		inline Ref<DirectionalLight> GetDirectionalLight() { return m_DirectionalLight; }

		PointLight& GetPointLight() { return m_PointLight; }
	private:
		Ref<DirectionalLight> m_DirectionalLight;
		PointLight m_PointLight;

	};

}