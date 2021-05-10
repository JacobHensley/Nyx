#include "ObjectPanel.h"
#include <glm/gtc/type_ptr.hpp>
#include <imgui/misc/cpp/imgui_stdlib.h>

namespace Nyx {

	ObjectPanel::ObjectPanel(Ref<Scene> scene)
		:	m_Scene(scene)
	{
	}

	ObjectPanel::ObjectPanel()
	{
	}

	void ObjectPanel::Render(SceneObject selectedObject)
	{
		ImGui::Begin("Object Properties");

		if (selectedObject)
		{
			// Set display name to TagComponent if there is one
			std::string name = "NULL";
			if (selectedObject.HasComponent<TagComponent>())
			{
				name = selectedObject.GetComponent<TagComponent>();
			}

			// Name input field
			std::string nameInput = name;
			if (ImGui::InputText("##NameInput", &nameInput, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
			{
				if (nameInput != name && selectedObject.HasComponent<TagComponent>())
					selectedObject.GetComponent<TagComponent>().Tag = nameInput;
			}

			// Transform component
			if (selectedObject.HasComponent<TransformComponent>() && ImGui::CollapsingHeader("Transform"))
			{
				TransformComponent& transformComponent = selectedObject.GetComponent<TransformComponent>();

				ImGui::DragFloat3("Translation", glm::value_ptr(transformComponent.Translation), 0.1f);
				ImGui::DragFloat3("Scale", glm::value_ptr(transformComponent.Scale), 0.1f);
				ImGui::DragFloat3("Rotation", glm::value_ptr(transformComponent.Rotation), 0.1f);
			}

			// Mesh component
			if (selectedObject.HasComponent<MeshComponent>() && ImGui::CollapsingHeader("Mesh"))
			{
			}

			// Material component
			if (selectedObject.HasComponent<MaterialComponent>() && ImGui::CollapsingHeader("Material"))
			{
			}
		}

		ImGui::End();
	}

}