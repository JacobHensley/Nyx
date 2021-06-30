#include "NXpch.h"
#include "Nyx/Scene/SceneObject.h"
#include "Nyx/Scene/Components.h"
#include "Nyx/Graphics/SceneRenderer.h"

namespace Nyx {

	Scene::Scene()
		:	m_Path("")
	{
	}

	void Scene::Update()
	{
	}

	void Scene::Render(Ref<Camera> camera)
	{
		SceneRenderer::Begin(this, camera);

		m_LightEnvironment = CreateRef<LightEnvironment>();
		m_EnvironmentMap = CreateRef<EnvironmentMap>();
		
		// Directional Lights
		auto directionalLightComponents = m_Registry.group<DirectionalLightComponent>(entt::get<TransformComponent>);
		for (auto object : directionalLightComponents) 
		{
			auto [transform, directionalLightComponent] = directionalLightComponents.get<TransformComponent, DirectionalLightComponent>(object);

			Ref<DirectionalLight> light = CreateRef<DirectionalLight>();
			light->Direction = transform.Rotation;
			light->Radiance = directionalLightComponent.Radiance;
			light->Active = directionalLightComponent.Active;

			m_LightEnvironment->AddDirectionalLight(light);
		}

		// Point Lights
		auto pointLightComponents = m_Registry.group<PointLightComponent>(entt::get<TransformComponent>);
		for (auto object : pointLightComponents)
		{
			auto [transform, pointLightComponent] = pointLightComponents.get<TransformComponent, PointLightComponent>(object);

			Ref<PointLight> light = CreateRef<PointLight>();
			light->Position = transform.Translation;
			light->Radiance = pointLightComponent.Radiance;
			light->Intensity = pointLightComponent.Intensity;
			light->Active = pointLightComponent.Active;

			m_LightEnvironment->AddPointLight(light);
		}

		// Environment Map
		auto environmentMapComponents = m_Registry.view<EnvironmentMapComponent>();
		for (auto object : environmentMapComponents)
		{
			auto& environmentMapComponent = environmentMapComponents.get<EnvironmentMapComponent>(object);
			m_EnvironmentMap->irradianceMap = environmentMapComponent.IrradianceMap;
			m_EnvironmentMap->radianceMap = environmentMapComponent.RadianceMap;
		}

		SceneRenderer::SetEnvironment(m_EnvironmentMap, m_LightEnvironment);

		// Meshes
		int i = 0;
		auto group = m_Registry.group<MeshComponent>(entt::get<TransformComponent>);
		for (auto object : group)
		{
			auto [transform, mesh] = group.get<TransformComponent, MeshComponent>(object);

			SceneRenderer::SubmitMesh(mesh.GetMesh(), transform.GetTransform());

			if (object == m_SelectedObject)
				SceneRenderer::SetSelectedObject(i);

			i++;
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