#pragma once
#include <glm/glm.hpp>

namespace Nyx {

	struct DirectionalLight
	{
		glm::vec3 Radiance;
		float padding01;
		glm::vec3 Direction;
		float padding02;

		DirectionalLight() = default;
		DirectionalLight(glm::vec3 radiance, glm::vec3 direction)
			: Radiance(radiance), Direction(direction)
		{
		}
	};

	struct PointLight
	{
		glm::vec3 Position;
		float padding03;
		glm::vec3 Radiance;
		float Intensity;

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

		inline void SetDirectionalLight(DirectionalLight light) { m_DirectionalLight = light; }
		inline DirectionalLight& GetDirectionalLight() { return m_DirectionalLight; }

		inline void SetPointLight(PointLight light) { m_PointLight = light;}
		PointLight& GetPointLight() { return m_PointLight; }

	private:
		DirectionalLight m_DirectionalLight;
		PointLight m_PointLight;

	};

}