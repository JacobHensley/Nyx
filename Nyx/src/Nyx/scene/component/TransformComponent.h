#pragma once
#include "Nyx/scene/component/Component.h"
#include "glm/glm.hpp"

namespace Nyx {

	class TransformComponent : public Component
	{
	public:
		TransformComponent(const glm::mat4& transform);
	public:
		void Rotate(float degrees, glm::vec3 axis);
		void Translate(glm::vec3 delta);
		void Scale(glm::vec3 scaler);

		glm::mat4& GetTransform() { return m_Transform; }
		void SetTransform(glm::mat4 transform) { m_Transform = transform; }
		static Component::Type* GetStaticType()
		{
			static Component::Type type = { "Transform" };
			return &type;
		}

		inline virtual Component::Type* GetType() const override { return GetStaticType(); }

	public:
		glm::mat4 m_Transform;
	};

}