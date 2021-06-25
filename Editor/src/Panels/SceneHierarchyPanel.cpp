#include "SceneHierarchyPanel.h"
#include <imgui/imgui.h>

namespace Nyx {

	SceneHierarchyPanel::SceneHierarchyPanel()
	{
	}

	void SceneHierarchyPanel::Render(Ref<Scene>& scene, SceneObject& selectedObject)
	{
		ImGui::Begin("Scene");

		static int selected = -1;
		int index = 0;

		// Iterate though all objects in the scene
		scene->GetRegistry().each([&](auto objectID)
		{
			// Check if object is vaild
			SceneObject object = { objectID, scene.get() };
			if (!object)
				return;

			// Check if selectedObject has changed and update selected accordingly
			if (object == selectedObject)
				selected = index;

			// Check for click on object and update selectedObject
			if (ImGui::Selectable((object.GetObjectName() + "##" + std::to_string(index)).c_str(), selected == index) && selected != index)
			{
				selected = index;
				selectedObject = object;
			}

			// Delete object context menu
			ImGui::OpenPopupOnItemClick(("ObjectContextMenu##" + std::to_string(index)).c_str(), 1);
			if (ImGui::BeginPopup(("ObjectContextMenu##" + std::to_string(index)).c_str()))
			{
				ImGui::Text(object.GetObjectName().c_str());
				ImGui::Separator();

				if (ImGui::Selectable("Delete"))
				{
					scene->Remove(object);
					if (selectedObject == object)
						selectedObject = {};
				}

				ImGui::EndPopup();
			}

			index++;
		});

		// Open window popup if the user right clicks on the window
		if (ImGui::IsMouseClicked(1) && ImGui::IsWindowHovered() && !ImGui::IsItemHovered())
		{
			ImGui::OpenPopup("WindowPopup");
		}

		// Add object popup
		if (ImGui::BeginPopup("WindowPopup"))
		{
			ImGui::Text("Add Object");
			ImGui::Separator();

			if (ImGui::Selectable("Add empty object"))
			{
				selectedObject = scene->CreateObject("Untitled Object");
			}

			if (ImGui::Selectable("Add mesh object"))
			{
				selectedObject = scene->CreateObject("Untitled Object");

				// Load defaul mesh if it does not already exist
				AssetHandle defaultMesh = AssetManager::Load<Mesh>("assets/models/Cube1m.fbx");
				selectedObject.AddComponent<MeshComponent>(defaultMesh);
			}

			ImGui::EndPopup();
		}

		// Check if the user clicks on the window but not on the buttons to deselect the object
		if (ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered())
		{
			selected = -1;
			selectedObject = {};
		}

		ImGui::End();
	}

}