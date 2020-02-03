#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace Nyx {

	struct Light
	{
		glm::vec3 radiance;
		glm::vec3 direction;

		bool active = true;

		Light(glm::vec3 radiance, glm::vec3 direction, bool active = true)
			: radiance(radiance), direction(direction), active(active)
		{
		}

		Light()
			: radiance(glm::vec3()), direction(glm::vec3()), active(true)
		{
		}
	};

	class LightEnvironment 
	{
	public:
		LightEnvironment();
		~LightEnvironment();

		void AddLight(Light* light);
		bool RemoveLight(Light* light);

		inline std::vector<Light*>& GetLights() { return m_Lights; }

	private:
		std::vector<Light*> m_Lights;

	};

}