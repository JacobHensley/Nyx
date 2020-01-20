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

	public:
		void Init(Scene* Scene);

		void Update();
		void Render();
		
		inline void Toggle(bool active) { m_IsActive = active; }
		inline bool IsActive() { return m_IsActive; }

		void AddComponent(Component* component);

		bool operator==(const SceneObject& other) const {
			return false;
		}

	private:
		Scene* m_Scene = nullptr;
		bool m_IsActive = true;
	};

}