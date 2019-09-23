#include "NXpch.h"
#include "Scene.h"
#include "component/MeshComponent.h"
#include "component/TransformComponent.h"

namespace Nyx {

	Scene::Scene()
	{
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

			TransformComponent*  transformComp = m_ComponentCache.Get<TransformComponent>(meshComp->GetSceneObject());
			NX_ASSERT(transformComp, "Transform Component is invaild");
			meshComp->RenderMesh(transformComp->GetTransform());
		}
	}

	void Scene::AddObject(SceneObject& object)
	{
		object.Init(this);
		m_SceneObjects.push_back(object);
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