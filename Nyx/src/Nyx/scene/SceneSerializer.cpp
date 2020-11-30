#include "NXpch.h"
#include "SceneSerializer.h"
#include "glm/gtx/matrix_decompose.hpp"
#include "glm/gtx/quaternion.hpp"
#include "yaml-cpp/yaml.h"
#include "component/MeshComponent.h"
#include "component/TransformComponent.h"
#include "component/MaterialComponent.h"
#include "component/ScriptComponent.h"

template<>
struct YAML::convert<glm::vec3> {
	static YAML::Node encode(const glm::vec3& rhs) {
		YAML::Node node;
		node.push_back(rhs.x);
		node.push_back(rhs.y);
		node.push_back(rhs.z);
		return node;
	}

	static bool decode(const YAML::Node& node, glm::vec3& rhs) {
		if (!node.IsSequence() || node.size() != 3) {
			return false;
		}

		rhs.x = node[0].as<float>();
		rhs.y = node[1].as<float>();
		rhs.z = node[2].as<float>();
		return true;
	}
};

template<>
struct YAML::convert<glm::vec4> {
	static YAML::Node encode(const glm::vec4& rhs) {
		YAML::Node node;
		node.push_back(rhs.x);
		node.push_back(rhs.y);
		node.push_back(rhs.z);
		node.push_back(rhs.w);
		return node;
	}

	static bool decode(const YAML::Node& node, glm::vec4& rhs) {
		if (!node.IsSequence() || node.size() != 4) {
			return false;
		}

		rhs.x = node[0].as<float>();
		rhs.y = node[1].as<float>();
		rhs.z = node[2].as<float>();
		rhs.w = node[3].as<float>();
		return true;
	}

};

template<>
struct YAML::convert<glm::quat> {
	static YAML::Node encode(const glm::quat& rhs) {
		YAML::Node node;
		node.push_back(rhs.w);
		node.push_back(rhs.x);
		node.push_back(rhs.y);
		node.push_back(rhs.z);
		return node;
	}

	static bool decode(const YAML::Node& node, glm::quat& rhs) {
		if (!node.IsSequence() || node.size() != 4) {
			return false;
		}

		rhs.w = node[0].as<float>();
		rhs.x = node[1].as<float>();
		rhs.y = node[2].as<float>();
		rhs.z = node[3].as<float>();
		return true;
	}

};

namespace Nyx
{
	YAML::Emitter& operator << (YAML::Emitter& out, const glm::quat& q) {
		out << YAML::Flow;
		out << YAML::BeginSeq << q.w << q.x << q.y << q.z << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator << (YAML::Emitter& out, const glm::vec3& v) {
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	void SceneSerializer::Save(Ref<Scene>& scene, const String& path)
	{
		YAML::Emitter out;

		out << YAML::BeginMap;
		out << YAML::Key << "IrradianceMap";
		out << YAML::Value << scene->GetEnvironmentMap()->irradianceMap.GetUUID();
		out << YAML::Key << "RadianceMap";
		out << YAML::Value << scene->GetEnvironmentMap()->radianceMap.GetUUID();
		out << YAML::EndMap;

		out << YAML::BeginMap;
		out << YAML::Key << "Objects";
		out << YAML::Value;
		out << YAML::BeginSeq;

		auto objects = scene->GetSceneObjects();

		for (uint i = 0; i < objects.size(); i++)
		{
			Ref<SceneObject> object = objects[i];

			out << YAML::BeginMap;

			out << YAML::Key << "Object UUID";
			out << YAML::Value << object->GetUUID();

			if (true)
			{
				out << YAML::Key << "Name";
				out << YAML::Value << object->GetObjectName();
			}

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
			out << YAML::Key << "UUID";
			out << YAML::Value << (UUID)uuid;
			out << YAML::Key << "Asset Type";
			out << YAML::Value << AssetManager::GetAssetTypeName(asset->m_AssetType);

			if (assetPaths.find(uuid) != assetPaths.end()) {
				out << YAML::Key << "Path";
				out << YAML::Value << assetPaths.at(uuid);
			}

			out << YAML::EndMap;
		}

		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(path);
		fout << out.c_str();
	}

	Ref<Scene> SceneSerializer::Load(const String& path)
	{
		std::vector<YAML::Node> nodes = YAML::LoadAllFromFile(path);

		YAML::Node assets = nodes[2]["Assets"];
		uint size = assets.size();
		for (int i = 0; i < assets.size(); i++)
		{
			uint64_t UUID = assets[i]["UUID"].as<uint64_t>();
			String path = assets[i]["Path"] ? assets[i]["Path"].as<String>() : "";

			if (path != "")
			{
				String type = assets[i]["Asset Type"].as<String>();
				if (type == "Mesh")
					AssetManager::InsertAndLoad<Mesh>(UUID, path);
				if (type == "TextureCube")
					AssetManager::InsertAndLoad<TextureCube>(UUID, path);
			}

		}

		YAML::Node envMap = nodes[0];
		uint64_t irradianceMapUUID = envMap["IrradianceMap"].as<uint64_t>();
		uint64_t radianceMapUUID = envMap["RadianceMap"].as<uint64_t>();

		Ref<LightEnvironment> lightEnvironment = CreateRef<LightEnvironment>();
		lightEnvironment->SetDirectionalLight(CreateRef<DirectionalLight>(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, -0.5f, -0.75f)));
		Ref<EnvironmentMap> map = CreateRef<EnvironmentMap>(AssetHandle(radianceMapUUID), AssetHandle(irradianceMapUUID));
		Ref<Camera> cam = CreateRef<Camera>(glm::perspectiveFov(glm::radians(45.0f), 1280.0f, 720.0f, 0.01f, 1000.0f));
		Ref<Scene> scene = CreateRef<Scene>(cam, map, lightEnvironment);

		YAML::Node objects = nodes[1]["Objects"];

		for (int i = 0; i < objects.size(); i++)
		{
			String name = objects[i]["Name"].as<String>();
			uint64_t UUID = objects[i]["Object UUID"].as<uint64_t>();

			Ref<SceneObject> object = scene->CreateObject(name);

			object->SetUUID(UUID);

			YAML::Node transformComponent = objects[i]["TransformComponent"];
			if (transformComponent)
			{
				glm::vec3 translation = transformComponent["Translation"].as<glm::vec3>();
				glm::vec3 scale = transformComponent["Scale"].as<glm::vec3>();
				glm::quat rotation = transformComponent["Rotation"].as<glm::quat>();

				glm::mat4 transform = glm::mat4(1.0f);
			//	transform = glm::translate(transform, translation);
			//	transform = glm::scale(transform, scale);
				glm::mat4 rot = glm::toMat4(rotation);
				transform = glm::translate(glm::mat4(1.0f), translation) * rot * glm::translate(glm::mat4(1.0f), scale);

				Ref<TransformComponent> component = CreateRef<TransformComponent>(transform);
				object->AddComponent(component);
			}

			YAML::Node meshComponent = objects[i]["MeshComponent"];
			if (meshComponent)
			{
				uint64_t meshUUID = meshComponent.as<uint64_t>();
				AssetHandle handle(meshUUID);

				Ref<MeshComponent> meshComponent = CreateRef<MeshComponent>(handle);
				object->AddComponent(meshComponent);
			}

			YAML::Node materialComponent = objects[i]["MaterialComponent"];
			if (materialComponent)
			{
				uint64_t materialUUID = materialComponent.as<uint64_t>();
			}
		}

		return scene;
	}
}