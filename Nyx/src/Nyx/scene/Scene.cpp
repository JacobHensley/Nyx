#include "NXpch.h"
#include "Scene.h"
#include "component/MeshComponent.h"
#include "component/TransformComponent.h"
#include "component/MaterialComponent.h"
#include "component/ScriptComponent.h"
#include "Nyx/graphics/renderer/SceneRenderer.h"
#include "Nyx/graphics/MeshFactory.h"
#include "glm/gtx/matrix_decompose.hpp"
#include "glm/gtx/quaternion.hpp"

namespace Nyx {

	Scene::Scene(Ref<Camera> camera, Ref<EnvironmentMap> environmentMap, Ref<LightEnvironment> lightEnvironment)
		: m_Camera(camera), m_EnvironmentMap(environmentMap), m_LightEnvironment(lightEnvironment)
	{
	}

	void Scene::Update()
	{
		m_Camera->Update();

		if (m_SceneObjects.size() > 0)
		{
			for (auto object : m_SceneObjects)
			{
				object->Update();
			}
		}
	}

	void Scene::Render()
	{
		SceneRenderer::Begin(this);

		for (auto object : m_SceneObjects)
		{
			object->Render();
		}

		SceneRenderer::End();
		SceneRenderer::Flush();
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

	bool Scene::SetSelectedObject(const Ref<SceneObject>& sceneObject)
	{
		auto e = std::find(m_SceneObjects.begin(), m_SceneObjects.end(), sceneObject);

		m_SelectedObject = sceneObject;

		return false;
	}

	void Scene::Remove(const Ref<SceneObject>& sceneObject)
	{
		auto e = std::find(m_SceneObjects.begin(), m_SceneObjects.end(), sceneObject);

		if (e != m_SceneObjects.end())
		{
			if (sceneObject == m_SelectedObject)
				m_SelectedObject = nullptr;
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