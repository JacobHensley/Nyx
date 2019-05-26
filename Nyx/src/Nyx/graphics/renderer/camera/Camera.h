#pragma once
#include "glm/glm.hpp"

namespace Nyx {

	class Camera
	{
	public:
		Camera(const glm::mat4 projectionMatrix);
		~Camera();

		void Update();

		inline const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		inline void SetProjectionMatrix(const glm::mat4& projectionMatrix) { m_ProjectionMatrix = projectionMatrix; }

		inline const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		inline void SetViewMatrix(const glm::mat4& viewMatrix) { m_ViewMatrix = viewMatrix; }

		inline const glm::vec3& GetPosition() const { return m_Position; }
		inline void SetPosition(const glm::vec3& position) { m_Position = position; }

		inline void Translate(const glm::vec3& translation) { m_Position += translation; }
	public:
		glm::vec3 m_Position;
		
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;

		glm::vec2 m_LastMousePosition;

		float m_Pitch = 0, m_Yaw = -90.0f;
		float m_MovementSpeed = 0.05f, m_Sensitivity = 0.175;

		glm::vec3 m_CameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 m_Up = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 m_CameraFront = glm::vec3(0.0f, 0.0f, -1.0f);

		glm::vec3 m_CameraDirection = glm::normalize(m_Position - m_CameraTarget);
		glm::vec3 m_CameraRight = glm::normalize(glm::cross(m_Up, m_CameraDirection));
		glm::vec3 m_CameraUp = glm::cross(m_CameraDirection, m_CameraRight);
	};

}