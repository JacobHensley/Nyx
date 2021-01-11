#pragma once
#include <glm/glm.hpp>

struct Triangle
{
	glm::vec3 Points[3];

	Triangle()
	{
		Points[0] = glm::vec3(0);
		Points[1] = glm::vec3(0);
		Points[2] = glm::vec3(0);
	}

	Triangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3)
	{
		Points[0] = p1;
		Points[1] = p2;
		Points[2] = p3;
	}

};