#pragma once
#include "Nyx/Asset/AssetManager.h"
#include "Nyx/Graphics/Material.h"
#include "Nyx/graphics/LightEnvironment.h"
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <string>

namespace Nyx {

	struct TransformComponent
	{
		glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f }; // Radians
		glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;

		glm::mat4 GetTransform() const
		{
			glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));
			return glm::translate(glm::mat4(1.0f), Translation) * rotation * glm::scale(glm::mat4(1.0f), Scale);
		}
	};

	struct TagComponent
	{
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag)
			:	Tag(tag)
		{
		}

		operator std::string& () { return Tag; }
		operator const std::string& () const { return Tag; }
	};

	struct MeshComponent
	{
		AssetHandle Mesh;

		MeshComponent() = default;
		MeshComponent(const MeshComponent&) = default;
		MeshComponent(AssetHandle mesh)
			: Mesh(mesh)
		{
		}

		Ref<Nyx::Mesh> GetMesh() { return Mesh.Get<Nyx::Mesh>(); }
		UUID GetUUID() { return Mesh.GetUUID(); }
	};

	struct SelectedComponent
	{
		SelectedComponent() = default;
		SelectedComponent(const SelectedComponent&) = default;
	};

	struct DirectionalLightComponent
	{
		glm::vec3 Radiance = { 1.0f, 1.0f, 1.0f };
		uint32_t Active = true;

		DirectionalLightComponent() = default;
		DirectionalLightComponent(const DirectionalLightComponent&) = default;
	};

	struct PointLightComponent
	{
		glm::vec3 Radiance = { 1.0f, 1.0f, 1.0f };
		float Intensity = 1.0f;
		uint32_t Active = true;

		PointLightComponent() = default;
		PointLightComponent(const PointLightComponent&) = default;
	};

	struct SpotLightComponent
	{
		glm::vec3 Radiance = { 1.0f, 1.0f, 1.0f };
		float InnerCutoff = 25.0f; // Degrees
		float OuterCutoff = 45.0f;
		float Intensity = 1.0f;
		uint32_t Active = true;

		SpotLightComponent() = default;
		SpotLightComponent(const SpotLightComponent&) = default;
	};

	struct EnvironmentMapComponent
	{
		AssetHandle RadianceMap;
		AssetHandle IrradianceMap;

		Ref<TextureCube> GetRadianceMap() { return RadianceMap.Get<TextureCube>(); }
		Ref<TextureCube> GetIrradianceMap() { return IrradianceMap.Get<TextureCube>(); }

		EnvironmentMapComponent() = default;
		EnvironmentMapComponent(const EnvironmentMapComponent&) = default;
		EnvironmentMapComponent(AssetHandle radianceMap, AssetHandle irradianceMap)
			: RadianceMap(radianceMap), IrradianceMap(irradianceMap)
		{
		}
		EnvironmentMapComponent(AssetHandle radianceMap)
			: RadianceMap(radianceMap)
		{
			IrradianceMap = AssetManager::Insert(radianceMap.Get<TextureCube>()->CalculateIrradianceMap());
		}
	};
}