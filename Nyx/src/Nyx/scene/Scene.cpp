#include "NXpch.h"
#include "Nyx/Scene/SceneObject.h"
#include "Nyx/Scene/Components.h"
#include "Nyx/Graphics/SceneRenderer.h"

namespace Nyx {

	Scene::Scene()
	{
	}

	void Scene::Update()
	{
	}

	void Scene::Render(Ref<Camera> camera)
	{
		SceneRenderer::Begin(this, camera);

		auto directionalLightComponents = m_Registry.view<DirectionalLightComponent>();
		for (auto object : directionalLightComponents) {
			auto& directionalLightComponent = directionalLightComponents.get<DirectionalLightComponent>(object);
			SceneRenderer::SubmitDirectionalLight(directionalLightComponent.Light);
		}

		auto pointLightComponents = m_Registry.view<PointLightComponent>();
		for (auto object : pointLightComponents) {
			auto& pointLightComponent = pointLightComponents.get<PointLightComponent>(object);
			SceneRenderer::SubmitPointLight(pointLightComponent.Light);
		}

		auto group = m_Registry.group<TransformComponent>(entt::get<MeshComponent>);
		for (auto object : group)
		{
			auto [transform, mesh] = group.get<TransformComponent, MeshComponent>(object);
			SceneRenderer::SubmitMesh(mesh.GetMesh(), transform.GetTransform());
		}

		SceneRenderer::End();
	}

	SceneObject Scene::CreateObject(const std::string& tag)
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