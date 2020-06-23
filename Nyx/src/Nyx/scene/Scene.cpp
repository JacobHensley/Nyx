#include "NXpch.h"
#include "Scene.h"
#include "component/MeshComponent.h"
#include "component/TransformComponent.h"
#include "component/MaterialComponent.h"
#include "component/ScriptComponent.h"
#include "Nyx/graphics/renderer/SceneRenderer.h"
#include "Nyx/graphics/MeshFactory.h"

#include "yaml-cpp/yaml.h"

namespace Nyx {

	Scene::Scene(Ref<Camera> camera, Ref<EnvironmentMap> environmentMap, Ref<LightEnvironment> lightEnvironment)
		:	m_Camera(camera), m_EnvironmentMap(environmentMap), m_LightEnvironment(lightEnvironment)
	{
		YAML::Node config = YAML::LoadFile("assets/config/config.yaml");
	/*	NX_CORE_DEBUG("Size: {0}", config.size());
		NX_CORE_DEBUG("By name: {0}", config["Position"]);
		NX_CORE_DEBUG("By name at index 2: {0}", config["Position"][2]);
		NX_CORE_DEBUG("Name size: {0}", config["Position"]);
		float x = config["Position"][0].as<float>();
		config["Position"][0] = 4.4f;
		std::ofstream fout("config.yaml");
		fout << config; */
	}

	void Scene::Update()
	{
		for (auto object : m_SceneObjects)
		{
			object->Update();
		}
	}

	void Scene::Render()
	{
		SceneRenderer::Begin(this);

		for (auto object : m_SceneObjects)
		{
			object->Render();
		}

		SceneRenderer::Flush();
		SceneRenderer::End();
	}

	Ref<SceneObject> Scene::CreateObject(const String& debugName)
	{
		Ref<SceneObject> object = CreateRef<SceneObject>(debugName);
		m_SceneObjects.push_back(object);

		AddObject(object);
		return object;
	}

	Ref<SceneObject> Scene::CreateObject(const String& debugName, std::initializer_list<Ref<Component>> components)
	{
		Ref<SceneObject> object = CreateRef<SceneObject>(debugName);
		m_SceneObjects.push_back(object);

		AddObject(object);

		for (auto component : components)
		{
			object->AddComponent(component);
		}

		return object;
	}

	void Scene::Remove(const Ref<SceneObject>& sceneObject)
	{
		auto e = std::find(m_SceneObjects.begin(), m_SceneObjects.end(), sceneObject);

		if (e != m_SceneObjects.end())
		{
			m_SceneObjects.erase(e);
			return;
		}
	}

	void Scene::RemoveObject(uint index)
	{
		m_SceneObjects.erase(m_SceneObjects.begin() + index);
	}

	void Scene::AddObject(Ref<SceneObject> object)
	{
		object->Init(this);
	}
}