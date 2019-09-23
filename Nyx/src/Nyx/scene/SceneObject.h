#pragma once
#include "component/Component.h"
#include "component/ComponentCache.h"

namespace Nyx {

	class Scene;

	class SceneObject
	{
	public:
		SceneObject();
		~SceneObject();

		void Init(Scene* Scene);
		void Update();
		void Render();

		void AddComponent(Component* component);

	private:
		Scene* m_Scene = nullptr;
	};

}