#pragma once
#include "SceneObject.h"

namespace Nyx {

	class Scene
	{
	public:
		Scene();
		~Scene();

		void Update();
		void Render();

		void AddObject(SceneObject& object);
		void RemoveAll();

		void AddComponent(Component* component);

	private:
		ComponentCache m_ComponentCache;
		std::vector<SceneObject> m_SceneObjects;
	};

}