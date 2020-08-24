#pragma once
#include "SceneObject.h"
#include "Nyx/graphics/renderer/Camera.h"
#include "Nyx/graphics/renderer/LightEnvironment.h"
#include "Nyx/Asset.h"

namespace Nyx {

	class Mesh;
	class Material;

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
		Scene(Ref<Camera> camera, Ref<EnvironmentMap> environmentMap, Ref<LightEnvironment> lightEnvironment);

	public:
		void Update();
		void Render();

		Ref<SceneObject> CreateObject(const String& debugName);
		Ref<SceneObject> CreateObject(const String& debugName, std::initializer_list<Ref<Component>> components);
		
		void Save(const String& filename);

		void Remove(const Ref<SceneObject>& sceneObject);
		void RemoveObject(uint index);

		inline std::vector<Ref<SceneObject>>& GetSceneObjects() { return m_SceneObjects; }
		const Ref<Camera> GetCamera() { return m_Camera; }
		Ref<LightEnvironment> GetLightEnvironment() { return m_LightEnvironment; }

		Ref<EnvironmentMap> GetEnvironmentMap() { return m_EnvironmentMap; }
		inline void SetEnvironmentMap(Ref<EnvironmentMap> environmentMap) { m_EnvironmentMap = environmentMap; }
		
	private:
		void AddObject(Ref<SceneObject> object);

	private:
		std::vector<Ref<SceneObject>> m_SceneObjects;
		Ref<Camera> m_Camera;
		Ref<EnvironmentMap> m_EnvironmentMap;
		Ref<LightEnvironment> m_LightEnvironment;

	};
}