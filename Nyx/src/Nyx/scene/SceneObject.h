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
		
		inline void SetVisibility(bool visible) { m_Visibility = visible; }
		inline bool IsVisible() { return m_Visibility; }

		void AddComponent(Component* component);

		bool operator==(const SceneObject& other) const {
			return false;
		}

	private:
		Scene* m_Scene = nullptr;
		bool m_Visibility = true;
	};

}