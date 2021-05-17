#include "EditorLayer.h"
#include <glm/gtc/type_ptr.hpp>

namespace Nyx {

	EditorLayer::EditorLayer()
		: Layer("Editor")
	{
		Init();
	}

	void EditorLayer::Init()
	{
		m_EditorCamera = CreateRef<Camera>(glm::perspectiveFov(glm::radians(45.0f), 1280.0f, 720.0f, 0.1f, 100.0f));
		m_Scene = SceneSerializer::Load("assets/scenes/Shapes.nyx");

		m_ViewportPanel = ViewportPanel();
		m_HierarchyPanel = SceneHierarchyPanel(m_Scene);
		m_ObjectPanel = ObjectPanel(m_Scene);
	}

	void EditorLayer::Update()
	{
		m_Scene->Update();
		m_EditorCamera->Update();
	}

	void EditorLayer::Render()
	{
		m_Scene->Render(m_EditorCamera);
	}

	void EditorLayer::ImGUIRender()
	{
		m_ViewportPanel.Render(m_EditorCamera, m_SelectedObject);
		m_HierarchyPanel.Render(m_SelectedObject);
		m_ObjectPanel.Render(m_SelectedObject);

		SceneRenderer::OnImGuiRender();

		ImGui::Begin("Scene Settings");
		ImGui::SliderFloat3("Point Light", glm::value_ptr(m_Scene->GetLightEnvironment()->GetPointLight().Position), -1, 1);
		ImGui::SliderFloat3("Dir Light", glm::value_ptr(m_Scene->GetLightEnvironment()->GetDirectionalLight().Direction), -1, 1);
		ImGui::End();
	}

	void EditorLayer::OnEvent(Event& e)
	{
		if (e.GetEventType() == KeyPressed)
		{
			if (((KeyPressedEvent&)e).GetKeycode() == NX_KEY_G)
				 m_ViewportPanel.SetGizmoMode(ImGuizmo::TRANSLATE);
			if (((KeyPressedEvent&)e).GetKeycode() == NX_KEY_R)
				 m_ViewportPanel.SetGizmoMode(ImGuizmo::ROTATE);
			if (((KeyPressedEvent&)e).GetKeycode() == NX_KEY_S)
				 m_ViewportPanel.SetGizmoMode(ImGuizmo::SCALE);
		}		
	}

}