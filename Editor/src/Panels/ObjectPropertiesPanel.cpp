#include "ObjectPropertiesPanel.h"
#include "Nyx/Scene/Scene.h"
#include "Nyx/Scene/Components.h"
#include "Nyx/Utilities/Utilities.h"
#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

ObjectPropertiesPanel::ObjectPropertiesPanel()
{
}

void ObjectPropertiesPanel::Draw(Nyx::Ref<Nyx::SceneObject> object)
{
	if (!object)
		return;

	ImGui::Begin("Object Properties");

	DrawNameInput(object);
	ImGui::SameLine();
	DrawAddComponents(object);

	if (object->HasComponent<Nyx::TransformComponent>() && ImGui::CollapsingHeader("Transform"))
	{
		Nyx::TransformComponent& transformComponent = object->GetComponent<Nyx::TransformComponent>();

		ImGui::DragFloat3("Translation", glm::value_ptr(transformComponent.Translation), 0.1f);
		ImGui::DragFloat3("Scale", glm::value_ptr(transformComponent.Scale), 0.1f);
		ImGui::DragFloat3("Rotation", glm::value_ptr(transformComponent.Rotation), 0.1f);
	}

	if (object->HasComponent<Nyx::MeshComponent>() && ImGui::CollapsingHeader("Mesh"))
	{
		Nyx::MeshComponent& meshComponent = object->GetComponent<Nyx::MeshComponent>();

		Nyx::Ref<Nyx::Mesh> mesh = meshComponent.GetMesh();
		const std::string& path = mesh->GetPath();
		std::string meshInput = path;
		meshInput.reserve(128);

		if (ImGui::Button(" ... "))
		{
			std::string file = Nyx::OpenFileExplorer("FBX\0*.FBX\0");
			if (file != "")
			{
				if (file != path)
					meshComponent.Mesh = Nyx::AssetManager::Load<Nyx::Mesh>(file);
				else
					mesh->Reload(file);
			}
		}

		ImGui::SameLine();

		ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 70);
		if (ImGui::InputText("##MeshInput", &meshInput, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
		{
			if (meshInput != path)
			{
				std::string newString = meshInput.c_str();
				meshComponent.Mesh = Nyx::AssetManager::Load<Nyx::Mesh>(newString);
			}
		}
	}

	if (object->HasComponent<Nyx::MaterialComponent>() && ImGui::CollapsingHeader("Material"))
	{
		Nyx::MaterialComponent& materialComponent = object->GetComponent<Nyx::MaterialComponent>();
	}

	ImGui::End();
}

void ObjectPropertiesPanel::DrawNameInput(Nyx::Ref<Nyx::SceneObject> object)
{
	std::string name = "Untitled Object";
	if (object->HasComponent<Nyx::TagComponent>())
	{
		name = object->GetComponent<Nyx::TagComponent>();
	}

	std::string nameInput = name;
	if (ImGui::InputText("##NameInput", &nameInput, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
	{
		if (object->HasComponent<Nyx::TagComponent>())
			object->GetComponent<Nyx::TagComponent>().Tag = nameInput;
	}
}

void ObjectPropertiesPanel::DrawAddComponents(Nyx::Ref<Nyx::SceneObject> object)
{
	if (ImGui::Button("Add Component"))
	{
		ImGui::OpenPopup("AddComponents");
	}

	if (ImGui::BeginPopup("AddComponents"))
	{
		ImGui::Text("Components");
		ImGui::Separator();

		if (ImGui::Selectable("Transform"))
		{
			object->AddComponent<Nyx::TransformComponent>();
		}

		if (ImGui::Selectable("Mesh"))
		{
			object->AddComponent<Nyx::MeshComponent>();
		}

		if (ImGui::Selectable("Material"))
		{
			object->AddComponent<Nyx::MaterialComponent>();
		}

		ImGui::EndPopup();
	}
}