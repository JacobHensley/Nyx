#pragma once
#include "Nyx/Scene/SceneObject.h"
#include "Nyx/Asset/Asset.h"
#include "Nyx/Graphics/Camera.h"
#include "Nyx/graphics/LightEnvironment.h"
#include <entt/include/entt.hpp>
#include <String>

namespace Nyx {

	class Scene
	{
	public:
		Scene();

	public:
		void Update();
		void Render(Ref<Camera> camera);

		SceneObject CreateObject();
		SceneObject CreateObject(const std::string& tag);
		void Remove(SceneObject& sceneObject);

		inline void SetSelectedObject(SceneObject object) { m_SelectedObject = object; }

		inline entt::registry& GetRegistry() { return m_Registry; }

		inline const std::string& GetPath() { return m_Path; }
		inline void SetPath(const std::string& path) { m_Path = path; }

	private:
		entt::registry m_Registry;
		std::string m_Path;

		SceneObject m_SelectedObject;

		Ref<LightEnvironment> m_LightEnvironment;
		Ref<EnvironmentMap> m_EnvironmentMap;

		friend class SceneObject;
	};
}