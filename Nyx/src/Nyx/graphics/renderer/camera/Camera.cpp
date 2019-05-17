#include "NXpch.h"
#include "Camera.h"

namespace Nyx {

	Camera::Camera(const glm::mat4 projectionMatrix)
		:	m_ProjectionMatrix(projectionMatrix)
	{
		m_ViewMatrix = glm::mat4(1.0f); //Identity Matrix
		m_Position = glm::vec3(0.0f, 0.0f, 0.0f);
		m_Rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	}

	Camera::~Camera()
	{
	}

}