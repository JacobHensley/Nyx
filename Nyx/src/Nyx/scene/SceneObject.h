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

		void RenderSelected();

		void AddComponent(Ref<Component> component);

		template<typename T>
		Ref<T> GetComponent()
		{
			if (m_Components.find(T::GetStaticType()) != m_Components.end())
				return std::static_pointer_cast<T>(m_Components[T::GetStaticType()]);

			return nullptr;
		}

		template<typename T>
		bool RemoveComponent()
		{
			if (m_Components.find(T::GetStaticType()) != m_Components.end())
			{
				m_Components.erase(T::GetStaticType());
				return true;
			}

			return false;
		}

		inline void SetActive(bool value) { m_IsActive = value; }
		inline bool IsActive() { return m_IsActive; }

		inline UUID GetUUID() { return m_UUID; }
		inline void SetUUID(uint64_t id) { m_UUID = id; }

		inline const std::unordered_map<Component::Type*, Ref<Component>>& GetComponents() { return m_Components; }
		
		inline void SetObjectName(const String& name) { m_ObjectName = name; }
		inline const String& GetObjectName() { return m_ObjectName; }

	private:
		Scene* m_Scene = nullptr;
		String m_ObjectName;

		UUID m_UUID;

		bool m_IsActive = true;

		std::unordered_map<Component::Type*, Ref<Component>> m_Components;
	};

}