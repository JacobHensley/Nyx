#include "NXpch.h"
#include "SceneObject.h"
#include "Scene.h"
#include "Nyx/scene/SceneRenderer.h"
#include "Nyx/scene/component/MeshComponent.h"
#include "Nyx/scene/component/MaterialComponent.h"

namespace Nyx {

	SceneObject::SceneObject()
		: m_DebugName("No name")
	{
	}

	SceneObject::SceneObject(const String& debugName)
		: m_DebugName(debugName)
	{
	}

	SceneObject::SceneObject(const SceneObject& other)
		: m_DebugName(other.m_DebugName)
	{
	}

	void SceneObject::Init(Scene* scene)
	{
		m_Scene = scene;
	}

	void SceneObject::Update()
	{
		if (m_IsActive)
		{
			for (auto const& x : m_Components)
			{
				x.second->OnUpdate();
			}
		}
	}

	void SceneObject::Render()
	{
		if (m_IsActive)
		{
			Ref<MeshComponent> meshComponent = GetComponent<MeshComponent>();
			Ref<TransformComponent> transformComponent = GetComponent<TransformComponent>();
			Ref<MaterialComponent> materialComponent = GetComponent<MaterialComponent>();

			if (materialComponent)
				SceneRenderer::SubmitMesh(meshComponent->GetMesh(), transformComponent->GetTransform(), materialComponent->GetMaterial());
			else
				SceneRenderer::SubmitMesh(meshComponent->GetMesh(), transformComponent->GetTransform());
		

			for (auto const& x : m_Components)
			{
				x.second->OnRender();
			}

		}
	}

	void SceneObject::AddComponent(Ref<Component> component)
	{
		m_Components[component->GetType()] = component;
		component->SetSceneObject(this);
	}

}