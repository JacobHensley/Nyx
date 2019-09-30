#include "NXpch.h"
#include "TransformComponent.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>

namespace Nyx {

	TransformComponent::TransformComponent(const glm::mat4& transform)
		: m_Transform(transform)
	{

	}

	void TransformComponent::Rotate(float degrees, glm::vec3 axis)
	{
		m_Transform = glm::rotate(m_Transform, glm::radians(degrees), axis);
	}

	void TransformComponent::Translate(glm::vec3 delta)
	{
		m_Transform = glm::translate(m_Transform, delta);
	}

	void TransformComponent::Scale(glm::vec3 scaler)
	{
		m_Transform = glm::scale(m_Transform, scaler);
	}

}