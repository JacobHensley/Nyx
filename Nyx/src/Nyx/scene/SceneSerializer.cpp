#include "NXpch.h"
#include "SceneSerializer.h"
#include "Nyx/Scene/Components.h"

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

namespace Nyx
{

	YAML::Emitter& operator << (YAML::Emitter& out, const glm::vec3& v) {
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	void SceneSerializer::Save(Ref<Scene>& scene, const std::string& path)
	{
		YAML::Emitter out;

		SerializeAssets(out);
		SerializeObjects(scene, out);

		std::ofstream fout(path);
		fout << out.c_str();
	}

	Ref<Scene> SceneSerializer::Load(const std::string& path)
	{
		std::vector<YAML::Node> nodes = YAML::LoadAllFromFile(path);

		Ref<Scene> scene = CreateRef<Scene>();

		YAML::Node assets = nodes[0];
		LoadAssets(assets);

		YAML::Node objects = nodes[1];
		LoadObjects(scene, objects);

		return scene;
	}

	void SceneSerializer::SerializeAssets(YAML::Emitter& out)
	{
		out << YAML::BeginSeq;

		auto assetPaths = AssetManager::GetAssetPaths();
		auto AssetUUIDs = AssetManager::GetAssets();

		for (auto const& [uuid, asset] : AssetUUIDs)
		{
			if (assetPaths.find(uuid) != assetPaths.end())
			{
				out << YAML::BeginMap;

				out << YAML::Key << "UUID" << YAML::Value << (UUID)uuid;
				out << YAML::Key << "Asset Type" << YAML::Value << AssetManager::GetAssetTypeName(asset->m_AssetType);
				out << YAML::Key << "Path" << YAML::Value << assetPaths.at(uuid);

				out << YAML::EndMap;
			}
		}

		out << YAML::EndSeq;
	}

	void SceneSerializer::SerializeObjects(Ref<Scene>& scene, YAML::Emitter& out)
	{
		out << YAML::BeginSeq;

		scene->GetRegistry().each([&](auto objectID)
		{
			SceneObject object = { objectID, scene.get() };
			if (!object)
				return;

			SerializeObject(out, object);
		});

		out << YAML::EndSeq;
	}

	void SceneSerializer::SerializeObject(YAML::Emitter& out, SceneObject object)
	{
		out << YAML::BeginMap;

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

			out << YAML::Key << "Mesh UUID" << YAML::Value << meshComponent.GetUUID();

			out << YAML::EndMap;
		}

		if (object.HasComponent<PointLightComponent>())
		{
			out << YAML::Key << "PointLightComponent";
			out << YAML::BeginMap;

			auto& pointLightComponent = object.GetComponent<PointLightComponent>();

			out << YAML::Key << "Radiance"  << YAML::Value << pointLightComponent.Radiance;
			out << YAML::Key << "Intensity" << YAML::Value << pointLightComponent.Intensity;
			out << YAML::Key << "Active"    << YAML::Value << pointLightComponent.Active;

			out << YAML::EndMap;
		}

		if (object.HasComponent<DirectionalLightComponent>())
		{
			out << YAML::Key << "DirectionalLightComponent";
			out << YAML::BeginMap;

			auto& directionalLightComponent = object.GetComponent<DirectionalLightComponent>();

			out << YAML::Key << "Radiance" << YAML::Value << directionalLightComponent.Radiance;
			out << YAML::Key << "Active"   << YAML::Value << directionalLightComponent.Active;

			out << YAML::EndMap;
		}

		if (object.HasComponent<SpotLightComponent>())
		{
			out << YAML::Key << "SpotLightComponent";
			out << YAML::BeginMap;

			auto& spotLightComponent = object.GetComponent<SpotLightComponent>();

			out << YAML::Key << "Radiance" << YAML::Value << spotLightComponent.Radiance;
			out << YAML::Key << "InnerCutoff" << YAML::Value << spotLightComponent.InnerCutoff;
			out << YAML::Key << "OuterCutoff" << YAML::Value << spotLightComponent.OuterCutoff;
			out << YAML::Key << "Intensity" << YAML::Value << spotLightComponent.Intensity;
			out << YAML::Key << "Active" << YAML::Value << spotLightComponent.Active;

			out << YAML::EndMap;
		}

		if (object.HasComponent<EnvironmentMapComponent>())
		{
			out << YAML::Key << "EnvironmentMapComponent";
			out << YAML::BeginMap;

			auto& environmentMapComponent = object.GetComponent<EnvironmentMapComponent>();

			out << YAML::Key << "RadianceMap UUID" << YAML::Value << environmentMapComponent.RadianceMap.GetUUID();

			out << YAML::EndMap;
		}

		out << YAML::EndMap;
	}

	void SceneSerializer::LoadAssets(YAML::Node assets)
	{
		for (int i = 0; i < assets.size(); i++)
		{
			uint64_t UUID = assets[i]["UUID"].as<uint64_t>();
			std::string type = assets[i]["Asset Type"].as<std::string>();
			std::string path = assets[i]["Path"].as<std::string>();
			
			if (type == "Mesh")
				AssetManager::TryInsertAndLoad<Mesh>(UUID, path);
			else if (type == "Texture")
				AssetManager::TryInsertAndLoad<Texture>(UUID, path);
			else if (type == "TextureCube")
				AssetManager::TryInsertAndLoad<TextureCube>(UUID, path); 
		}
	}

	void SceneSerializer::LoadObjects(Ref<Scene>& scene, YAML::Node objects)
	{
		for (int i = 0; i < objects.size(); i++)
		{
			SceneObject object = scene->CreateObject();

			YAML::Node tagComponent = objects[i]["TagComponent"];
			if (tagComponent)
			{
				TagComponent& component = object.AddComponent<TagComponent>();

				component.Tag = tagComponent["Tag"].as<std::string>();
			}

			YAML::Node transformComponent = objects[i]["TransformComponent"];
			if (transformComponent)
			{
				TransformComponent& component = object.AddComponent<TransformComponent>();

				component.Translation = transformComponent["Translation"].as<glm::vec3>();
				component.Rotation = transformComponent["Rotation"].as<glm::vec3>();
				component.Scale = transformComponent["Scale"].as<glm::vec3>();
			}

			YAML::Node meshComponent = objects[i]["MeshComponent"];
			if (meshComponent)
			{
				MeshComponent& component = object.AddComponent<MeshComponent>();

				uint64_t UUID = meshComponent["Mesh UUID"].as<uint64_t>();
				component.Mesh = AssetHandle(UUID);
			}

			YAML::Node pointLightComponent = objects[i]["PointLightComponent"];
			if (pointLightComponent)
			{
				PointLightComponent& component = object.AddComponent<PointLightComponent>();
				
				component.Radiance = pointLightComponent["Radiance"].as<glm::vec3>();
				component.Intensity = pointLightComponent["Intensity"].as<float>();
				component.Active = pointLightComponent["Active"].as<uint32_t>();
			}

			YAML::Node directionalLightComponent = objects[i]["DirectionalLightComponent"];
			if (directionalLightComponent)
			{
				DirectionalLightComponent& component = object.AddComponent<DirectionalLightComponent>();

				component.Radiance = directionalLightComponent["Radiance"].as<glm::vec3>();
				component.Active = directionalLightComponent["Active"].as<uint32_t>();
			}

			YAML::Node spotLightComponent = objects[i]["SpotLightComponent"];
			if (spotLightComponent)
			{
				SpotLightComponent& component = object.AddComponent<SpotLightComponent>();

				component.Radiance = spotLightComponent["Radiance"].as<glm::vec3>();
				component.InnerCutoff = spotLightComponent["InnerCutoff"].as<float>();
				component.OuterCutoff = spotLightComponent["OuterCutoff"].as<float>();
				component.Intensity = spotLightComponent["Intensity"].as<float>();
				component.Active = spotLightComponent["Active"].as<uint32_t>();
			}

			YAML::Node environmentMapComponent = objects[i]["EnvironmentMapComponent"];
			if (environmentMapComponent)
			{
				EnvironmentMapComponent& component = object.AddComponent<EnvironmentMapComponent>();

				uint64_t UUID = environmentMapComponent["RadianceMap UUID"].as<uint64_t>();
				component.RadianceMap = AssetHandle(UUID);
				component.IrradianceMap = AssetManager::Insert(AssetManager::Get<TextureCube>(component.RadianceMap)->CalculateIrradianceMap());
			}
		}
	}

}