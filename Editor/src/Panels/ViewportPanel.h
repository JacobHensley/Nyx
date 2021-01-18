#pragma once
#include "Nyx/Core/Core.h"
#include "Nyx/Scene/Scene.h"
#include "Nyx/Graphics/Camera.h"
#include <imgui/imgui.h>
#include <imgui/ImGuizmo.h>
#include <glm/glm.hpp>

class ViewportPanel
{
public:
	ViewportPanel();

	void Draw(Nyx::Ref<Nyx::Camera>& camera);

	glm::vec2 GetNDC();

	const glm::vec2& GetSize() const { return m_Size; }
	const glm::vec2& GetPosition() const  { return m_Position; }

	bool IsHovered() const { return m_Hovered; }
	bool IsFocused() const { return m_Focused; }

	Nyx::SceneObject GetGizmoObject() { return m_GizmoObject; };
	void SetGizmoObject(Nyx::SceneObject object) { m_GizmoObject = object; }

	void SetGizmoMode(ImGuizmo::OPERATION mode) { m_GizmoMode = mode; }

private:
	void DrawGizmo(Nyx::Ref<Nyx::Camera>& camera);

private:
	glm::vec2 m_Size;
	glm::vec2 m_LastSize;
	glm::vec2 m_Position;

	bool m_Hovered;
	bool m_Focused;

	Nyx::SceneObject m_GizmoObject;
	ImGuizmo::OPERATION m_GizmoMode = ImGuizmo::TRANSLATE;
};