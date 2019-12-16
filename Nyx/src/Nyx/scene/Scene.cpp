#include "NXpch.h"
#include "Scene.h"
#include "component/MeshComponent.h"
#include "component/TransformComponent.h"

namespace Nyx {

	Scene::Scene()
	{
		NX_CORE_INFO("Created Scene");
	}

	Scene::~Scene()
	{
		RemoveAll();
	}

	void Scene::Update()
	{
		for (SceneObject& object : m_SceneObjects)
			object.Update();
	}

	void Scene::Render()
	{
		auto& meshComponents = m_ComponentCache.GetAll<MeshComponent>();

		for (auto component : meshComponents)
		{
			MeshComponent* meshComp = (MeshComponent*)component;

			NX_ASSERT(meshComp->GetSceneObject(), "Scene Object is null");

			SceneObject* object = meshComp->GetSceneObject();

			if (object->IsVisible())
			{
				TransformComponent*  transformComp = m_ComponentCache.Get<TransformComponent>(meshComp->GetSceneObject());
				NX_ASSERT(transformComp, "Transform Component is invaild");
				meshComp->RenderMesh(transformComp->GetTransform());
			}

		}
	}

	SceneObject* Scene::CreateObject()
	{
		SceneObject* object = new SceneObject();
		AddObject(*object);
		return object;
	}

	SceneObject* Scene::CreateObject(std::initializer_list<Component*> components)
	{
		SceneObject* object = new SceneObject();
		AddObject(*object);
		
		for (auto component : components) 
		{
			object->AddComponent(component);
		}

		return object;
	}

	void Scene::AddObject(SceneObject& object)
	{
		object.Init(this);
		m_SceneObjects.push_back(object);
		NX_CORE_INFO("Added Object to Scene");
	}

	void Scene::Remove(const SceneObject& sceneObject)
	{
		auto e = std::find(m_SceneObjects.begin(), m_SceneObjects.end(), sceneObject);
		if (e != m_SceneObjects.end()) 
		{
			m_SceneObjects.erase(e);
			NX_CORE_INFO("Removed Object to Scene");
			return;
		}
		NX_CORE_INFO("Failed to Remove Object to Scene");
	}

	void Scene::RemoveAll()
	{
		m_ComponentCache.ClearAll();
	}

	void Scene::AddComponent(Component* component)
	{
		m_ComponentCache.Add(component);
	}

}