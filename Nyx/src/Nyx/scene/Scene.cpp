#include "NXpch.h"
#include "Scene.h"
#include "Nyx/graphics/renderer/SceneRenderer.h"
#include "Components.h"
#include "glm/gtx/matrix_decompose.hpp"
#include "glm/gtx/quaternion.hpp"

namespace Nyx {

	Scene::Scene(Ref<EnvironmentMap> environmentMap, Ref<LightEnvironment> lightEnvironment)
		: m_EnvironmentMap(environmentMap), m_LightEnvironment(lightEnvironment)
	{
	}

	void Scene::Update()
	{
	}

	void Scene::Render(Ref<Camera> camera)
	{
		SceneRenderer::Begin(this, camera);

		auto group = m_Registry.group<TransformComponent>(entt::get<MeshComponent>);

		for (auto object : group)
		{
			auto [transform, mesh] = group.get<TransformComponent, MeshComponent>(object);
			SceneRenderer::SubmitMesh(mesh.GetMesh(), transform.GetTransform());
		}

		SceneRenderer::End();
		SceneRenderer::Flush();
	}

	SceneObject Scene::CreateObject(const String& tag)
	{
		SceneObject sceneObject(m_Registry.create(), this);
		sceneObject.AddComponent<TagComponent>(tag);
		sceneObject.AddComponent<TransformComponent>();
		return sceneObject;
	}

	SceneObject Scene::CreateObject()
	{
		SceneObject sceneObject(m_Registry.create(), this);
		return sceneObject;
	}

	void Scene::Remove(SceneObject& sceneObject)
	{
		m_Registry.destroy(sceneObject);
	}
}