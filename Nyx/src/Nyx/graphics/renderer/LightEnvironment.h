#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

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

	class LightEnvironment 
	{
	public:
		LightEnvironment();
		~LightEnvironment();

		inline void SetDirectionalLight(Ref<DirectionalLight>& light) { m_DirectionalLight = light; }
		inline Ref<DirectionalLight> GetDirectionalLight() { return m_DirectionalLight; }

	private:
		Ref<DirectionalLight> m_DirectionalLight;

	};

}