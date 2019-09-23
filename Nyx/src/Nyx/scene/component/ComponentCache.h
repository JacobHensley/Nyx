#pragma once
#include "NXpch.h"
#include "Nyx/scene/component/Component.h"

namespace Nyx {

	class ComponentCache
	{
	private:
		std::unordered_map<Component::Type*, std::vector<Component*>> m_Components;
		std::unordered_map<SceneObject*, std::unordered_map<Component::Type*, Component*>> m_SceneObjectComponents;
	public:

		template<typename T>
		const std::vector<Component*>& GetAll()
		{
			//TODO: Add static assert. 
			return m_Components[T::GetStaticType()];
		}

		template<typename T>
		T* Get(const SceneObject* sceneObject)
		{
			//TODO: Add static assert.
			return (T*)m_SceneObjectComponents[(SceneObject*)sceneObject][T::GetStaticType()];
		}

		void Add(Component* component)
		{
			m_Components[component->GetType()].push_back(component);
			m_SceneObjectComponents[component->GetSceneObject()][component->GetType()] = component;
		}

		void ClearAll()
		{
			m_Components.clear();
			m_SceneObjectComponents.clear();
		}

	};

}