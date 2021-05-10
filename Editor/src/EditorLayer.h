#pragma once
#include "Nyx.h"
#include "Panels/ViewportPanel.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ObjectPanel.h"

namespace Nyx {

	class EditorLayer : public Layer
	{
	public:
		EditorLayer();

	public:
		void Init();

		void Update();
		void Render();
		void ImGUIRender();

		void OnEvent(Event& e);

	private:
		Ref<Scene> m_Scene;
		Ref<Camera> m_EditorCamera;

		SceneObject m_SelectedObject;

		ViewportPanel m_ViewportPanel;
		SceneHierarchyPanel m_HierarchyPanel;
		ObjectPanel m_ObjectPanel;
	};

}