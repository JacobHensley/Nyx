#pragma once
#include "Nyx/Scene/SceneObject.h"
#include "Nyx/Asset/Asset.h"
#include "Nyx/Graphics/Camera.h"
#include "Nyx/Graphics/LightEnvironment.h"
#include <entt/include/entt.hpp>
#include <String>

namespace Nyx {

	struct EnvironmentMap
	{
		EnvironmentMap(AssetHandle radianceMap, AssetHandle irradianceMap)
			:	radianceMap(radianceMap), irradianceMap(irradianceMap)
		{
		}

		AssetHandle radianceMap;
		AssetHandle irradianceMap;
	};

	class Scene
	{
	public:
		Scene(Ref<EnvironmentMap> environmentMap, Ref<LightEnvironment> lightEnvironment);

	public:
		void Update();
		void Render(Ref<Camera> camera);

		SceneObject CreateObject();
		SceneObject CreateObject(const std::string& tag);
		void Remove(SceneObject& sceneObject);

		inline entt::registry& GetRegistry() { return m_Registry; }

		inline SceneObject GetSelectedObject() { return m_SelectedObject; };
		inline void SetSelectedObject(entt::entity object) { m_SelectedObject = SceneObject(object, this); };

		inline const std::string& GetPath() { return m_Path; }
		inline void SetPath(const std::string& path) { m_Path = path; }

		Ref<LightEnvironment> GetLightEnvironment() { return m_LightEnvironment; }

		Ref<EnvironmentMap> GetEnvironmentMap() { return m_EnvironmentMap; }
		inline void SetEnvironmentMap(Ref<EnvironmentMap> environmentMap) { m_EnvironmentMap = environmentMap; }

	private:
		entt::registry m_Registry;
		std::string m_Path;

		Ref<EnvironmentMap> m_EnvironmentMap;
		Ref<LightEnvironment> m_LightEnvironment;
		
		SceneObject m_SelectedObject;
		
		friend class SceneObject;
	};
}