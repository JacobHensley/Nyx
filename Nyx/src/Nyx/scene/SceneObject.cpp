#include "NXpch.h"
#include "SceneObject.h"
#include "Scene.h"

namespace Nyx {

	SceneObject::SceneObject()
	{
	}

	SceneObject::~SceneObject()
	{
	}

	void SceneObject::Init(Scene* scene)
	{
		m_Scene = scene;
	}

	void SceneObject::Update()
	{
	}

	void SceneObject::Render()
	{
	}

	void SceneObject::AddComponent(Component* component)
	{
		NX_ASSERT(m_Scene, "Scene has not been initialized");

		component->SetSceneObject(this);
		m_Scene->AddComponent(component);
	}

}