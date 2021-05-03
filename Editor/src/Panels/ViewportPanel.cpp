#include "ViewportPanel.h"
#include "Nyx/Graphics/SceneRenderer.h"
#include "Nyx/Scene/Components.h"
#include "Nyx/Math/Math.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

ViewportPanel::ViewportPanel()
{
}

void ViewportPanel::Draw(Nyx::Ref<Nyx::Camera>& camera)
{
	ImGuiIO io = ImGui::GetIO();

	bool open = true;
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse;
	io.ConfigWindowsMoveFromTitleBarOnly = true;
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

	ImGui::Begin("Editor", &open, flags);

	ImVec2 size = ImGui::GetContentRegionAvail();
	m_Size = { size.x, size.y };

	ImVec2 position = ImGui::GetWindowPos();
	m_Position = { position.x, position.y };

	ImVec2 windowOffset = ImGui::GetCursorPos();
	m_Position.x += windowOffset.x;
	m_Position.y += windowOffset.y;

	m_Hovered = ImGui::IsWindowHovered();
	m_Focused = ImGui::IsWindowFocused();

	uint64_t textureID = Nyx::SceneRenderer::GetFinalBuffer()->GetColorAttachments()[0];
	ImGui::Image((void*)textureID, ImVec2(m_Size.x, m_Size.y), ImVec2::ImVec2(0, 1), ImVec2::ImVec2(1, 0));

	if (m_GizmoObject)
		DrawGizmo(camera);

	if (m_Size != m_LastSize)
	{
		Nyx::SceneRenderer::Resize(m_Size.x, m_Size.y);
		camera->SetProjectionMatrix(glm::perspectiveFov(glm::radians(45.0f), m_Size.x, m_Size.y, 0.01f, 1000.0f));
		m_LastSize = m_Size;
		Nyx::SceneRenderer::Resize(m_Size.x, m_Size.y);
	}

	ImGui::End();
	ImGui::PopStyleVar();
}

glm::vec2 ViewportPanel::GetNDC()
{
	auto [mouseX, mouseY] = ImGui::GetMousePos();
	mouseX -= m_Position.x;
	mouseY -= m_Position.y;

	glm::vec2 NDC = { (mouseX / m_Size.x) * 2.0f - 1.0f, ((mouseY / m_Size.y) * 2.0f - 1.0f) * -1.0f };

	return NDC;
}

void ViewportPanel::DrawGizmo(Nyx::Ref<Nyx::Camera>& camera)
{
	ImGuizmo::SetOrthographic(false);
	ImGuizmo::SetDrawlist();
	ImGuizmo::SetRect(m_Position.x, m_Position.y, m_Size.x, m_Size.y);

	if (!m_GizmoObject.HasComponent<Nyx::TransformComponent>())
		return;

	Nyx::TransformComponent& transformComponent = m_GizmoObject.GetComponent<Nyx::TransformComponent>();
	glm::mat4& transform = transformComponent.GetTransform();

	ImGuizmo::Manipulate(glm::value_ptr(camera->GetViewMatrix()), glm::value_ptr(camera->GetProjectionMatrix()), m_GizmoMode, ImGuizmo::LOCAL, glm::value_ptr(transform));

	glm::vec3 translation;
	glm::vec3 rotation;
	glm::vec3 scale;
	Nyx::DecomposeTransform(transform, translation, rotation, scale);

	transformComponent.Translation = translation;
	transformComponent.Rotation = rotation;
	transformComponent.Scale = scale;
}