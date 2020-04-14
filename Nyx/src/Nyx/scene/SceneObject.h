#pragma once
#include "component/Component.h"

namespace Nyx {

	class Scene;

	class SceneObject
	{
	public:
		SceneObject();
		SceneObject(const String& debugName);
		SceneObject(const SceneObject& other);

	public:
		void Init(Scene* Scene);

		void Update();
		void Render();

		void AddComponent(Ref<Component> component);

		template<typename T>
		Ref<T> GetComponent()
		{
			if (m_Components.find(T::GetStaticType()) != m_Components.end())
				return std::static_pointer_cast<T>(m_Components[T::GetStaticType()]);

			return nullptr;
		}

		inline void SetActive(bool value) { m_IsActive = value; }
		inline bool IsActive() { return m_IsActive; }

		inline const String& GetDebugName() { return m_DebugName; }

		bool operator==(const SceneObject& other) const {
			return false;
		}

	private:
		Scene* m_Scene = nullptr;
		String m_DebugName;

		bool m_IsActive = true;

		std::unordered_map<Component::Type*, Ref<Component>> m_Components;
	};

}