#include "NXpch.h"
#include "Scene.h"
#include "component/MeshComponent.h"
#include "component/TransformComponent.h"
#include "component/MaterialComponent.h"
#include "component/ScriptComponent.h"
#include "Nyx/graphics/renderer/SceneRenderer.h"
#include "Nyx/graphics/MeshFactory.h"
#include "glm/gtx/matrix_decompose.hpp"
#include "yaml-cpp/yaml.h"

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

	YAML::Emitter& operator << (YAML::Emitter& out, const glm::vec3& v) {
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator << (YAML::Emitter& out, const glm::quat& q) {
		out << YAML::Flow;
		out << YAML::BeginSeq << q.w << q.x << q.y << q.z << YAML::EndSeq;
		return out;
	}

	void Scene::Save(const String& filename)
	{
		NX_CORE_DEBUG(filename);

		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Objects";
		out << YAML::Value;
		out << YAML::BeginSeq;

		for (uint i = 0; i < m_SceneObjects.size(); i++)
		{
			Ref<SceneObject> object = m_SceneObjects[i];

			out << YAML::BeginMap;

			out << YAML::Key << "Object UUID";
			out << YAML::Value << object->GetUUID();

			Ref<TransformComponent> transformComponent = object->GetComponent<TransformComponent>();
			if (transformComponent)
			{
				glm::mat4& transform = transformComponent->GetTransform();
				glm::vec3 scale;
				glm::quat rotation;
				glm::vec3 translation;
				glm::vec3 skew;
				glm::vec4 perspective;
				glm::decompose(transform, scale, rotation, translation, skew, perspective);

				out << YAML::Key << "TransformComponent";
				out << YAML::Value;
				out << YAML::BeginMap;
				out << YAML::Key << "Translation";
				out << YAML::Value << translation;
				out << YAML::Key << "Scale";
				out << YAML::Value << scale;
				out << YAML::Key << "Rotation";
				out << YAML::Value << rotation;
				out << YAML::EndMap;
			}

			Ref<MeshComponent> meshComponent = object->GetComponent<MeshComponent>();
			if (meshComponent)
			{
				out << YAML::Key << "MeshComponent";
				out << YAML::Value << meshComponent->GetUUID();
			}

			Ref<MaterialComponent> materialComponent = object->GetComponent<MaterialComponent>();
			if (materialComponent)
			{
				out << YAML::Key << "MaterialComponent";
				out << YAML::Value << materialComponent->GetUUID();
			}

			out << YAML::EndMap;
		}

		out << YAML::EndSeq;
		out << YAML::EndMap;


		out << YAML::BeginMap;
		out << YAML::Key << "Assets";
		out << YAML::Value;
		out << YAML::BeginSeq;

		auto assetPaths = AssetManager::GetAssetPaths();
		auto AssetUUIDs = AssetManager::GetAssets();

		for (auto const& [uuid, asset] : AssetUUIDs)
		{
			out << YAML::BeginMap;
			out << YAML::Key << (UUID)uuid;
			out << YAML::Value;
			out << YAML::BeginMap;
			out << YAML::Key << "Asset Type";
			out << YAML::Value << AssetManager::GetAssetTypeName(asset->m_AssetType);

			if (assetPaths.find(uuid) != assetPaths.end()) {
				out << YAML::Key << "Path";
				out << YAML::Value << assetPaths.at(uuid);
			}

			out << YAML::EndMap;
			out << YAML::EndMap;
		}



		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(filename);
		fout << out.c_str();
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