#pragma once
#include "Nyx.h"

namespace Nyx {

	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel(Ref<Scene> scene);
		SceneHierarchyPanel();

		void Render(SceneObject& selectedObject);

	private:
		Ref<Scene> m_Scene;
	};

}