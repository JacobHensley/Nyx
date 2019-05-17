#pragma once
#include "glm/glm.hpp"

namespace Nyx {

	class Camera
	{
	public:
		Camera(const glm::mat4 projectionMatrix);
		~Camera();

		inline const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		inline void SetProjectionMatrix(const glm::mat4& projectionMatrix) { m_ProjectionMatrix = projectionMatrix; }

		inline const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		inline void SetViewMatrix(const glm::mat4& viewMatrix) { m_ViewMatrix = viewMatrix; }

		inline const glm::vec3& GetPosition() const { return m_Position; }
		inline void SetPosition(const glm::vec3& position) { m_Position = position; }

		inline const glm::vec3& GetRotation() const { return m_Rotation; }
		inline void SetRotation(const glm::vec3& rotation) { m_Rotation = rotation; }

		inline void Translate(const glm::vec3& translation) { m_Position += translation; }
		inline void Rotate(const glm::vec3& rotation) { m_Rotation += rotation; }
	private:
		glm::vec3 m_Position;
		glm::vec3 m_Rotation;

		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;
	};

}