#pragma once

#include <glm/glm.hpp>

namespace Nyx {

	struct Ray
	{
		glm::vec3 Origin, Direction;

		Ray()
			: Origin(0), Direction(0)
		{
		}

		Ray(const glm::vec3& origin, const glm::vec3& direction)
			: Origin(origin), Direction(direction)
		{
		}
	};

}
