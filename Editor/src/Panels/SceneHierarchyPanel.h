#pragma once
#include "Nyx.h"

namespace Nyx {

	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel();

		void Render(Ref<Scene>& scene, SceneObject& selectedObject);
	};

}