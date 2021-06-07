#pragma once
#include "Nyx.h"
#include <imgui/imgui.h>
#include <imgui/ImGuizmo.h>
#include <glm/glm.hpp>

namespace Nyx {

	class ViewportPanel
	{
	public:
		ViewportPanel();

	public:
		void Render(Ref<Camera> camera, SceneObject selectedObject);

		glm::vec2 GetMouseNDC();

		const glm::vec2& GetSize() const { return m_Size; }
		const glm::vec2& GetPosition() const { return m_Position; }

		bool IsHovered() const { return m_Hovered; }
		bool IsFocused() const { return m_Focused; }

		void SetGizmoMode(ImGuizmo::OPERATION mode) { m_GizmoMode = mode; }

	private:
		void RenderGizmo(Ref<Camera> camera, SceneObject selectedObject);

	private:
		glm::vec2 m_Size;
		glm::vec2 m_LastSize;
		glm::vec2 m_Position;

		bool m_Hovered;
		bool m_Focused;

		ImGuizmo::OPERATION m_GizmoMode = ImGuizmo::TRANSLATE;
	};

}