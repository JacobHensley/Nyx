#pragma once
#include "Nyx/scene/component/Component.h"
#include "glm/glm.hpp"

namespace Nyx {

	class TransformComponent : public Component
	{
	public:
		glm::mat4 m_Transform;

	public:
		TransformComponent(const glm::mat4& transform);

		const glm::mat4& GetTransform() { return m_Transform; }

		static Component::Type* GetStaticType()
		{
			static Component::Type type = { "Transform" };
			return &type;
		}

		inline virtual Component::Type* GetType() const override { return GetStaticType(); }

	};

}