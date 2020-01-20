#include "NXpch.h"
#include "Scene.h"
#include "component/MeshComponent.h"
#include "component/TransformComponent.h"
#include "component/MaterialComponent.h"
#include "SceneRenderer.h"
#include "Nyx/graphics/MeshFactory.h"

namespace Nyx {

	Scene::Scene(Camera* camera, TextureCube* skybox)
		: m_Camera(camera)
	{
		m_SkyboxMaterial = new Material(new Shader("assets/shaders/Skybox.shader"));
		m_SkyboxMaterial->SetTexture("u_SkyboxTexture", skybox);
		m_SkyboxMesh = MeshFactory::GenQuad(-1.0f, -1.0f, 0.0f, 2.0f, 2.0f);

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

		SceneRenderer::Begin(this);
		
		SceneRenderer::SubmitMesh(m_SkyboxMesh, glm::mat4(1.0f), false, m_SkyboxMaterial);

		for (auto component : meshComponents)
		{
			MeshComponent* meshComponent = (MeshComponent*)component;

			NX_ASSERT(meshComponent->GetSceneObject(), "Scene Object is null");
			SceneObject* object = meshComponent->GetSceneObject();
			
			if (object->IsActive())
			{
				TransformComponent* transformComponent = m_ComponentCache.Get<TransformComponent>(meshComponent->GetSceneObject());
				NX_ASSERT(transformComponent, "Transform Component is invaild");

				MaterialComponent* materialComponent = m_ComponentCache.Get<MaterialComponent>(meshComponent->GetSceneObject());

				if (materialComponent)
					SceneRenderer::SubmitMesh(meshComponent->GetMesh(), transformComponent->GetTransform(), true, materialComponent->GetMaterial());
				else
					SceneRenderer::SubmitMesh(meshComponent->GetMesh(), transformComponent->GetTransform());
			}
		}

		SceneRenderer::Flush();
		SceneRenderer::End();
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