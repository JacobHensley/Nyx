#include "NXpch.h"
#include "Scene.h"
#include "component/MeshComponent.h"
#include "component/TransformComponent.h"
#include "component/MaterialComponent.h"
#include "component/ScriptComponent.h"
#include "SceneRenderer.h"
#include "Nyx/graphics/MeshFactory.h"

namespace Nyx {


	Scene::Scene(Ref<Camera> camera, Ref<LightEnvironment> lightEnv, Ref<TextureCube> skybox)
		: m_Camera(camera), m_LightEnvironment(lightEnv)
	{
		m_SkyboxMaterial = CreateRef<Material>(CreateRef<Shader>("assets/shaders/Skybox.shader"));
		m_SkyboxMaterial->SetTexture("u_SkyboxTexture", skybox);
		m_SkyboxMaterial->SetDepthTesting(false);
		m_SkyboxMesh = MeshFactory::GenQuad(-1.0f, -1.0f, 0.0f, 2.0f, 2.0f);
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

		SceneRenderer::SubmitMesh(m_SkyboxMesh, glm::mat4(1.0f), m_SkyboxMaterial);

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

	Ref<SceneObject> Scene::CreateObject(std::initializer_list<Ref<Component>> components)
	{
		Ref<SceneObject> object = m_SceneObjects.emplace_back();
		AddObject(object);

		for (auto component : components)
		{
			object->AddComponent(component);
		}

		return object;
	}

	void Scene::AddObject(Ref<SceneObject> object)
	{
		object->Init(this);
	}

	void Scene::Remove(const Ref<SceneObject>& sceneObject)
	{
		auto e = std::find(m_SceneObjects.begin(), m_SceneObjects.end(), sceneObject);
		if (e != m_SceneObjects.end())
		{
			m_SceneObjects.erase(e);
			NX_CORE_INFO("Removed Object to Scene");
			return;
		}
	}

}


/*	Scene::Scene(Ref<Camera> camera, Ref<LightEnvironment> lightEnv, Ref<TextureCube> skybox)
		: m_Camera(camera), m_LightEnvironment(lightEnv)
	{
		m_SkyboxMaterial = CreateRef<Material>(CreateRef<Shader>("assets/shaders/Skybox.shader"));
		m_SkyboxMaterial->SetTexture("u_SkyboxTexture", skybox);
		m_SkyboxMaterial->SetDepthTesting(false);
		m_SkyboxMesh = MeshFactory::GenQuad(-1.0f, -1.0f, 0.0f, 2.0f, 2.0f);

		NX_CORE_INFO("Created Scene");
	}

	Scene::~Scene()
	{
	}

	void Scene::Update()
	{
		for (Ref<SceneObject> object : m_SceneObjects)
			object->Update();
	}

	void Scene::Render()
	{
		SceneRenderer::Begin(this);
		
		SceneRenderer::SubmitMesh(m_SkyboxMesh, glm::mat4(1.0f), m_SkyboxMaterial);

	/*	for (auto component : meshComponents)
		{
			Ref<MeshComponent> meshComponent = (MeshComponent*)component;

			NX_ASSERT(meshComponent->GetSceneObject(), "Scene Object is null");
			Ref<SceneObject> object = meshComponent->GetSceneObject();
			
			if (object->IsActive())
			{
				TransformComponent* transformComponent = m_ComponentCache.Get<TransformComponent>(meshComponent->GetSceneObject());
				NX_ASSERT(transformComponent, "Transform Component is invaild");

				MaterialComponent* materialComponent = m_ComponentCache.Get<MaterialComponent>(meshComponent->GetSceneObject());

				if (materialComponent)
					SceneRenderer::SubmitMesh(meshComponent->GetMesh(), transformComponent->GetTransform(), materialComponent->GetMaterial());
				else
					SceneRenderer::SubmitMesh(meshComponent->GetMesh(), transformComponent->GetTransform());
			}
		}

		auto& scriptComponents = m_ComponentCache.GetAll<ScriptComponent>();
		for (auto component : scriptComponents)
		{
			component->OnRender();
		}

		SceneRenderer::Flush();
		SceneRenderer::End();
	}

	Ref<SceneObject> Scene::CreateObject(const String& debugName)
	{
		Ref<SceneObject> object = m_SceneObjects.emplace_back(debugName);

		AddObject(object);
		return object;
	}

	Ref<SceneObject> Scene::CreateObject(std::initializer_list<Ref<Component>> components)
	{
		Ref<SceneObject> object = m_SceneObjects.emplace_back();
		AddObject(object);
		
		for (auto component : components) 
		{
			object->AddComponent(component);
		}

		return object;
	}

	void Scene::AddObject(Ref<SceneObject> object)
	{
		object->Init(this);
		NX_CORE_INFO("Added Object to Scene");
	}

	bool Scene::RemoveObject(uint index)
	{
		if (index <= m_SceneObjects.size())
		{
			m_ComponentCache.Delete(m_SceneObjects[index]);
			m_SceneObjects.erase(m_SceneObjects.begin() + index);
			return true;
		}
		return false;
	}

	void Scene::Remove(const Ref<SceneObject> sceneObject)
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

	void Scene::AddComponent(Ref<Component> component)
	{
		m_ComponentCache.Add(component);
		component->OnAttach();
	} */