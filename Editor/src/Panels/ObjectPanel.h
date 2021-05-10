#pragma once
#include "Nyx.h"

namespace Nyx {

	class ObjectPanel
	{
	public:
		ObjectPanel(Ref<Scene> scene);
		ObjectPanel();

		void Render(SceneObject selectedObject);

	private:
		Ref<Scene> m_Scene;
	};

}