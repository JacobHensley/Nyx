#pragma once
#include <glm/glm.hpp>

namespace Nyx {

	class Camera
	{
	public:
		Camera(const glm::mat4& projectionMatrix);

	public:
		void Update();
		void Reset();

		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		const glm::mat4 GetViewProjection() const { return m_ProjectionMatrix * m_ViewMatrix; }
		const glm::mat4 GetInverseVP() const { return glm::inverse(m_ProjectionMatrix * m_ViewMatrix); }

		const glm::vec3& GetPosition() const { return m_Position; }

		inline float GetExposure() const { return m_Exposure; }
		inline void SetExposure(float exposure) { m_Exposure = exposure; }

		inline void SetProjectionMatrix(const glm::mat4& projectionMatrix) { m_ProjectionMatrix = projectionMatrix; }
		inline void SetViewMatrix(const glm::mat4 viewMatrix) { m_ViewMatrix = viewMatrix; }

		inline float GetDistance() const { return m_Distance; }
		inline void SetDistance(float distance) { m_Distance = distance; }

	private:
		void MousePan(const glm::vec2& delta);
		void MouseRotate(const glm::vec2& delta);
		void MouseZoom(float delta);

		glm::vec3 GetUpDirection();
		glm::vec3 GetRightDirection();
		glm::vec3 GetForwardDirection();

		glm::vec3 CalculatePosition();
		glm::quat GetOrientation();

	private:
		glm::mat4 m_ProjectionMatrix, m_ViewMatrix;
		glm::vec3 m_Position, m_Rotation, m_FocalPoint;

		bool m_Panning, m_Rotating;
		glm::vec2 m_InitialMousePosition;
		glm::vec3 m_InitialFocalPoint, m_InitialRotation;

		float m_Distance;
		float m_PanSpeed, m_RotationSpeed, m_ZoomSpeed;

		float m_Pitch, m_Yaw;

		float m_Exposure = 1.0f;
		bool m_ExposureActive = false;
	};

}
