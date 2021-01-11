#pragma once
#include "Nyx/Asset/AssetManager.h"
#include "Nyx/Graphics/Material.h"
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

	struct MaterialComponent
	{
		AssetHandle Material;

		MaterialComponent() = default;
		MaterialComponent(const MaterialComponent&) = default;
		MaterialComponent(AssetHandle material)
			: Material(material)
		{
		}

		Ref<Nyx::Material> GetMaterial() { return Material.Get<Nyx::Material>(); }
		UUID GetUUID() { return Material.GetUUID(); }
	};

	struct SelectedComponent
	{
		bool Selected; //Do I even need this variable? or just if it has the component it is selected?

		SelectedComponent() = default;
		SelectedComponent(const SelectedComponent&) = default;
		SelectedComponent(bool selected)
			: Selected(selected)
		{
		}

		operator bool() { return Selected; }
	};
}