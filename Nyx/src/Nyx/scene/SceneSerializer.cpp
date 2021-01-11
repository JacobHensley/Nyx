#include "NXpch.h"
#include "SceneSerializer.h"
#include "Nyx/Scene/Components.h"
#include <yaml-cpp/yaml.h>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>

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

	void SceneSerializer::Save(Ref<Scene>& scene, const std::string& path)
	{
		YAML::Emitter out;

		//Asset Map will be moved into project file
		out << YAML::BeginMap; // Asset Map
		out << YAML::Key << "Assets";
		out << YAML::Value;
		out << YAML::BeginSeq;

		auto assetPaths = AssetManager::GetAssetPaths();
		auto AssetUUIDs = AssetManager::GetAssets();

		for (auto const& [uuid, asset] : AssetUUIDs)
		{
			out << YAML::BeginMap; // Asset

			out << YAML::Key << "UUID" << YAML::Value << (UUID)uuid;
			out << YAML::Key << "Asset Type" << YAML::Value << AssetManager::GetAssetTypeName(asset->m_AssetType);

			if (assetPaths.find(uuid) != assetPaths.end()) 
			{
				out << YAML::Key << "Path" << YAML::Value << assetPaths.at(uuid);
			}

			out << YAML::EndMap; // Asset
		}

		out << YAML::EndSeq;
		out << YAML::EndMap; // Asset Map


		out << YAML::BeginMap; // Scene Map

		//Save and load as one map
		out << YAML::Key << "RadianceMap UUID" << YAML::Value << scene->GetEnvironmentMap()->radianceMap.GetUUID();
		out << YAML::Key << "IrradianceMap UUID" << YAML::Value << scene->GetEnvironmentMap()->irradianceMap.GetUUID();

		//lights

		out << YAML::Key << "Objects";
		out << YAML::Value;
		out << YAML::BeginSeq; // Object List

		scene->GetRegistry().each([&](auto objectID)
		{
			SceneObject object = { objectID, scene.get() };
			if (!object)
				return;

			SerializeObject(out, object);
		});

		out << YAML::EndSeq; // Object List
		out << YAML::EndMap; // Scene Map

		std::ofstream fout(path);
		fout << out.c_str();
	}

	static void SerializeObject(YAML::Emitter& out, SceneObject object)
	{
		out << YAML::BeginMap; // Object

		if (object.HasComponent<TagComponent>())
		{
			out << YAML::Key << "TagComponent";
			out << YAML::BeginMap;

			auto& tag = object.GetComponent<TagComponent>().Tag;
			out << YAML::Key << "Tag" << YAML::Value << tag;

			out << YAML::EndMap;
		}

		if (object.HasComponent<TransformComponent>())
		{
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap;

			auto& transformComponent = object.GetComponent<TransformComponent>();
			out << YAML::Key << "Translation" << YAML::Value << transformComponent.Translation;
			out << YAML::Key << "Rotation"    << YAML::Value << transformComponent.Rotation;
			out << YAML::Key << "Scale"       << YAML::Value << transformComponent.Scale;

			out << YAML::EndMap;
		}

		if (object.HasComponent<MeshComponent>())
		{
			out << YAML::Key << "MeshComponent";
			out << YAML::BeginMap;

			auto& meshComponent = object.GetComponent<MeshComponent>();
			out << YAML::Key << "UUID" << YAML::Value << meshComponent.GetUUID();
			out << YAML::Key << "Path" << YAML::Value << meshComponent.GetMesh()->GetPath();

			out << YAML::EndMap;
		}

		if (object.HasComponent<MaterialComponent>())
		{
			out << YAML::Key << "MaterialComponent";
			out << YAML::BeginMap;

			auto& materialComponent = object.GetComponent<MaterialComponent>();
			out << YAML::Key << "UUID" << YAML::Value << materialComponent.GetUUID();
			out << YAML::Key << "Path" << YAML::Value << "NULL"; //Material will soon have their own file or maybe a index into a file of many materials

			out << YAML::EndMap;
		}

		out << YAML::EndMap; // Object
	}

	Ref<Scene> SceneSerializer::Load(const std::string& path)
	{
		std::vector<YAML::Node> nodes = YAML::LoadAllFromFile(path);

		YAML::Node assets = nodes[0]["Assets"];
		for (int i = 0; i < assets.size(); i++)
		{
			uint64_t UUID = assets[i]["UUID"].as<uint64_t>();
			std::string path = assets[i]["Path"] ? assets[i]["Path"].as<std::string>() : "";

			if (path != "")
			{
				std::string type = assets[i]["Asset Type"].as<std::string>();
				if (type == "Mesh")
					AssetManager::InsertAndLoad<Mesh>(UUID, path);
				else if (type == "Texture")
					AssetManager::InsertAndLoad<Texture>(UUID, path);
				else if (type == "TextureCube")
					AssetManager::InsertAndLoad<TextureCube>(UUID, path);
			}

		}

		//Load as one map
		uint64_t radianceMapUUID = nodes[1]["RadianceMap UUID"].as<uint64_t>();
		uint64_t irradianceMapUUID = nodes[1]["IrradianceMap UUID"].as<uint64_t>();
		Ref<EnvironmentMap> environmentMap = CreateRef<EnvironmentMap>(UUID(radianceMapUUID), UUID(irradianceMapUUID));

		//Load lights
		Ref<DirectionalLight> directionalLight = CreateRef<DirectionalLight>(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, -0.5f, -0.75f));
		Ref<LightEnvironment> lightEnvironment = CreateRef<LightEnvironment>();
		lightEnvironment->SetDirectionalLight(directionalLight);

		Ref<Scene> scene = CreateRef<Scene>(environmentMap, lightEnvironment);

		YAML::Node objects = nodes[1]["Objects"];
		for (int i = 0; i < objects.size(); i++)
		{
			SceneObject object = scene->CreateObject();

			YAML::Node tagComponent = objects[i]["TagComponent"];
			if (tagComponent)
			{
				std::string tag = tagComponent["Tag"].as<std::string>();
				object.AddComponent<TagComponent>(tag);
			}

			YAML::Node transformComponent = objects[i]["TransformComponent"];
			if (transformComponent)
			{
				TransformComponent& tc = object.AddComponent<TransformComponent>();

				tc.Translation = transformComponent["Translation"].as<glm::vec3>();
				tc.Rotation = transformComponent["Rotation"].as<glm::vec3>();
				tc.Scale = transformComponent["Scale"].as<glm::vec3>();
			}

			YAML::Node meshComponent = objects[i]["MeshComponent"];
			if (meshComponent)
			{
				uint64_t UUID = meshComponent["UUID"].as<uint64_t>();
				AssetHandle handle(UUID);

				MeshComponent& mc = object.AddComponent<MeshComponent>(handle);
			}

			YAML::Node materialComponent = objects[i]["MaterialComponent"];
			if (materialComponent)
			{
				uint64_t UUID = materialComponent["UUID"].as<uint64_t>();
				AssetHandle handle(UUID);

				MaterialComponent& mc = object.AddComponent<MaterialComponent>(handle);
			}
		}

		return scene;
	}
}