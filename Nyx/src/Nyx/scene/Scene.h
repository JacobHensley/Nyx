#pragma once
#include "SceneObject.h"

namespace Nyx {

	class Scene
	{
	public:
		Scene();
		~Scene();
	
	public:
		void Update();
		void Render();

		SceneObject* CreateObject();
		SceneObject* CreateObject(std::initializer_list<Component*> components);

		void AddObject(SceneObject& object);

		void Remove(const SceneObject& sceneObject);
		void RemoveAll();

		void AddComponent(Component* component);

		template<typename T>
		T* GetComponent(const SceneObject* sceneObject)
		{
			static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");
			return m_ComponentCache.Get<T>(sceneObject);
		}

	private:
		ComponentCache m_ComponentCache;
		std::vector<SceneObject> m_SceneObjects;

	};

}