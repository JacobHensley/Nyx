#pragma once
#include "Nyx/Core/Core.h"
#include <entt/include/entt.hpp>
#include <String>

namespace Nyx {

	class Scene;

	class SceneObject
	{
	public:
		SceneObject(entt::entity handle, Scene* scene)
			: m_Handle(handle), m_Scene(scene)
		{
		}

		SceneObject() = default;
		SceneObject(const SceneObject& other) = default;

	public:
		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			NX_ASSERT(!HasComponent<T>(), "Entity already has Component");
			return m_Scene->m_Registry.emplace<T>(m_Handle, std::forward<Args>(args)...);
		}

		template<typename T>
		bool HasComponent()
		{
			return m_Scene->m_Registry.has<T>(m_Handle);
		}

		template<typename T>
		T& GetComponent()
		{
			NX_ASSERT(HasComponent<T>(), "Entity does not have Component");
			return m_Scene->m_Registry.get<T>(m_Handle);
		}

		template<typename T>
		void RemoveComponent()
		{
			NX_ASSERT(HasComponent<T>(), "Entity does not have Component");
			m_Scene->m_Registry.remove<T>(m_Handle);
		}

		const std::string& GetObjectName();

		operator bool() const { return m_Handle != entt::null; }
		operator entt::entity() const { return m_Handle; }
		operator uint32_t() const { return (uint32_t)m_Handle; }

		bool operator==(const SceneObject& other) const
		{
			return m_Handle == other.m_Handle && m_Scene == other.m_Scene;
		}

		bool operator!=(const SceneObject& other) const
		{
			return !(*this == other);
		}

	private:
		entt::entity m_Handle = entt::null; // Should objects also have a UUID for saving?
		Scene* m_Scene = nullptr;
	};

}