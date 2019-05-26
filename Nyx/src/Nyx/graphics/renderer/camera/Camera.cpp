#include "NXpch.h"
#include "Camera.h"
#include "Nyx/input/KeyCodes.h"
#include "Nyx/Application.h"
#include "glm/gtc/matrix_transform.hpp"

namespace Nyx {

	Camera::Camera(const glm::mat4 projectionMatrix)
		:	m_ProjectionMatrix(projectionMatrix)
	{
		m_ViewMatrix = glm::mat4(1.0f);
		m_Position = glm::vec3(0.0f, 0.0f, 0.0f);
	}

	Camera::~Camera()
	{
	}

	void Camera::Update()
	{
		Input* input = Application::GetApp().GetWindow().GetInput();
		glm::vec2 mousePosition = input->GetMousePos();
		glm::vec2 mouseDelta = mousePosition - m_LastMousePosition;
		m_LastMousePosition = mousePosition;

		m_CameraDirection = glm::normalize(m_Position - m_CameraTarget);
		m_CameraRight = glm::normalize(glm::cross(m_Up, m_CameraDirection));
		m_CameraUp = glm::cross(m_CameraDirection, m_CameraRight);

		if (input->IsKeyPressed(NX_KEY_LEFT_ALT))
		{
			m_Yaw += mouseDelta.x * m_Sensitivity;
			m_Pitch += -mouseDelta.y * m_Sensitivity;

			if (m_Pitch > 89.0f)
				m_Pitch = 89.0f;
			if (m_Pitch < -89.0f)
				m_Pitch = -89.0f;
		}

		m_CameraFront.x = cos(glm::radians(m_Pitch)) * cos(glm::radians(m_Yaw));
		m_CameraFront.y = sin(glm::radians(m_Pitch));
		m_CameraFront.z = cos(glm::radians(m_Pitch)) * sin(glm::radians(m_Yaw));

		m_CameraFront = glm::normalize(m_CameraFront);

		if (!input->IsKeyPressed(NX_KEY_LEFT_CONTROL))
		{
			if (input->IsKeyPressed(NX_KEY_W))
				m_Position += m_CameraFront * m_MovementSpeed;
			if (input->IsKeyPressed(NX_KEY_S))
				m_Position -= m_CameraFront * m_MovementSpeed;
			if (input->IsKeyPressed(NX_KEY_A))
				m_Position -= glm::normalize(glm::cross(m_CameraFront, m_CameraUp)) * m_MovementSpeed;
			if (input->IsKeyPressed(NX_KEY_D))
				m_Position += glm::normalize(glm::cross(m_CameraFront, m_CameraUp)) * m_MovementSpeed;
		}

		if (input->IsKeyPressed(NX_KEY_LEFT_CONTROL))
		{
			if (input->IsKeyPressed(NX_KEY_W))
				m_Position += m_Up * m_MovementSpeed;
			if (input->IsKeyPressed(NX_KEY_S))
				m_Position -= m_Up * m_MovementSpeed;
		}

		m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_CameraFront, m_CameraUp);
	}

}