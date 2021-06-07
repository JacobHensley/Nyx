#include "ViewportPanel.h"
#include <glm/gtc/type_ptr.hpp>

namespace Nyx {

	ViewportPanel::ViewportPanel()
	{
	}

	void ViewportPanel::Render(Ref<Camera> camera, SceneObject selectedObject)
	{
		// Set window settings
		ImGuiIO io = ImGui::GetIO();
		io.ConfigWindowsMoveFromTitleBarOnly = true;
		bool open = true;
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin("Editor", &open, flags);

		// Set data
		ImVec2 size = ImGui::GetContentRegionAvail();
		m_Size = { size.x, size.y };

		ImVec2 position = ImGui::GetWindowPos();
		m_Position = { position.x, position.y };

		ImVec2 windowOffset = ImGui::GetCursorPos();
		m_Position.x += windowOffset.x;
		m_Position.y += windowOffset.y;

		m_Hovered = ImGui::IsWindowHovered();
		m_Focused = ImGui::IsWindowFocused();

		// Draw scene
		uint64_t textureID = SceneRenderer::GetFinalBuffer()->GetColorAttachments()[0];
		ImGui::Image((void*)textureID, ImVec2(m_Size.x, m_Size.y), ImVec2::ImVec2(0, 1), ImVec2::ImVec2(1, 0));

		// Check if there is a selected object and render gizmo for it
		if (selectedObject)
			RenderGizmo(camera, selectedObject);

		// Resize window
		if (m_Size != m_LastSize)
		{
			camera->SetProjectionMatrix(glm::perspectiveFov(glm::radians(45.0f), m_Size.x, m_Size.y, 0.01f, 1000.0f));
			m_LastSize = m_Size;
			Nyx::SceneRenderer::Resize((uint32_t)m_Size.x, (uint32_t)m_Size.y);
		}

		ImGui::End();
		ImGui::PopStyleVar();
	}

	glm::vec2 ViewportPanel::GetMouseNDC()
	{
		auto [mouseX, mouseY] = ImGui::GetMousePos();
		mouseX -= m_Position.x;
		mouseY -= m_Position.y;

		glm::vec2 NDC = { (mouseX / m_Size.x) * 2.0f - 1.0f, ((mouseY / m_Size.y) * 2.0f - 1.0f) * -1.0f };

		return NDC;
	}

	void ViewportPanel::RenderGizmo(Ref<Camera> camera, SceneObject selectedObject)
	{
		// Set gizmo settings
		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect(m_Position.x, m_Position.y, m_Size.x, m_Size.y);

		// Check for object transform
		if (!selectedObject.HasComponent<TransformComponent>())
			return;

		TransformComponent& transformComponent = selectedObject.GetComponent<TransformComponent>();
		glm::mat4& transform = transformComponent.GetTransform();

		// Render gizmo
		ImGuizmo::Manipulate(glm::value_ptr(camera->GetViewMatrix()), glm::value_ptr(camera->GetProjectionMatrix()), m_GizmoMode, ImGuizmo::LOCAL, glm::value_ptr(transform));

		// Decompose transform and apply it to object transform
		glm::vec3 translation;
		glm::vec3 rotation;
		glm::vec3 scale;
		DecomposeTransform(transform, translation, rotation, scale);

		transformComponent.Translation = translation;
		transformComponent.Rotation = rotation;
		transformComponent.Scale = scale;
	}

}