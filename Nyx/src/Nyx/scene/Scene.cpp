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
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Objects";
		out << YAML::Value;
		out << YAML::BeginSeq;

		for (Ref<SceneObject> object : m_SceneObjects)
		{
			out << YAML::BeginMap;
			out << YAML::Key << "Object";
			out << YAML::Value;
			out << YAML::BeginMap;
			out << YAML::Key << "Name";
			out << YAML::Value << object->GetObjectName();
			out << YAML::Key << "UUID";
			out << YAML::Value << object->GetUUID().GetUUID();
			out << YAML::Key << "Components";
			out << YAML::Value;
			out << YAML::BeginMap;
				
			Ref<TransformComponent> transformComponent = object->GetComponent<TransformComponent>();
			if (transformComponent)
			{
				out << YAML::Key << "Transform";
				out << YAML::Value;
				out << YAML::BeginMap;
				glm::mat4 transform = transformComponent->GetTransform();
				glm::vec3 scale;
				glm::quat rot;
				glm::vec3 trans;
				glm::vec3 scew;
				glm::vec4 prep;
				glm::decompose(transform, scale, rot, trans, scew, prep);

				out << YAML::Key << "Translation";
				out << YAML::Value << trans;
				out << YAML::Key << "Rotation";
				out << YAML::Value << rot;
				out << YAML::Key << "Scale";
				out << YAML::Value << scale;
				out << YAML::EndMap;
			}

			Ref<MeshComponent> meshComponent = object->GetComponent<MeshComponent>();
			if (meshComponent)
			{
				out << YAML::Key << "Mesh";
				out << YAML::Value;
				out << YAML::BeginMap;

				out << YAML::Key << "UUID";
				out << YAML::Value << meshComponent->GetUUID();

				out << YAML::EndMap;
			}

			out << YAML::EndMap;
			out << YAML::EndMap;

			out << YAML::EndMap;
		}

		out << YAML::EndSeq;
		out << YAML::EndMap;

		out << YAML::BeginMap;
		out << YAML::Key << "Assets";
		out << YAML::Value;
		out << YAML::BeginSeq;

		const std::unordered_map<UUID, String, UUIDHash>& paths = AssetManager::GetAssetPaths();

		for (auto const& x : paths)
		{
			UUID id = x.first;
			Ref<Asset> asset = AssetManager::GetByUUID(id);
			out << YAML::BeginMap;
			String typeName = AssetManager::GetAssetTypeName(asset->m_AssetType);
			out << YAML::Key << typeName;
			out << YAML::Value;
			out << YAML::BeginMap;

			out << YAML::Key << "UUID";
			out << YAML::Value << id;

			out << YAML::Key << "Path";
			out << YAML::Value << x.second;

			out << YAML::EndMap;
			out << YAML::EndMap;
		}

		out << YAML::EndSeq;
		out << YAML::EndMap;


		const String& path = "assets/config/" + filename;
		std::ofstream fout(path);
		fout << out.c_str();
	}

	void Scene::Remove(const Ref<SceneObject>& sceneObject)
	{
		auto e = std::find(m_SceneObjects.begin(), m_SceneObjects.end(), sceneObject);

		if (e != m_SceneObjects.end())
		{
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