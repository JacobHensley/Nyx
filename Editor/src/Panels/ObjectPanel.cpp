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

	static int selectedIndex = -1;
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
				MeshComponent& meshComponent = selectedObject.GetComponent<MeshComponent>();

				Ref<Mesh> mesh = meshComponent.GetMesh();
				const std::string& path = mesh->GetPath();
				std::string meshInput = path;
				meshInput.reserve(128);
				
				// Load Mesh button
				if (ImGui::Button(" ... "))
				{
					std::string file = OpenFileExplorer("FBX\0*.FBX\0");
					if (file != "")
					{
						if (file != path)
							meshComponent.Mesh = AssetManager::Load<Mesh>(file);
						else
							mesh->Reload();
					}
				}

				// Mesh path input field
				ImGui::SameLine();
				ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 70);
				if (ImGui::InputText("##MeshPathInput", &meshInput, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
				{
					if (meshInput != path)
					{
						std::string newString = meshInput.c_str();
						meshComponent.Mesh = AssetManager::Load<Mesh>(newString);
					}
				}

				// Material list
				std::vector<Ref<Material>> materials = mesh->GetMaterials();
				if (ImGui::ListBoxHeader("##MaterialList", materials.size()))
				{
					for (int i = 0; i < materials.size(); i++)
					{
						if (ImGui::Selectable(materials[i]->GetName().c_str(), i == selectedIndex))
						{
							selectedIndex = i;
						}
					}
					ImGui::ListBoxFooter();
					ImGui::Separator();
				}

				// Move to material window
				// -----------------------------------------------------------
				// Material viewer
				if (selectedIndex != -1)
				{
					ImGui::Text("%d", materials[selectedIndex]->HashBuffer());

					ImGui::Text(materials[selectedIndex]->GetShader()->GetPath().c_str());

					// Display all textures in material
					const std::vector<Ref<Texture>>& textures = materials[selectedIndex]->GetTextures();
					for (int i = 0; i < textures.size(); i++)
					{
						if (textures[i] != nullptr)
						{
							ImGui::Image((void*)textures[i]->GetTextureID(), ImVec2(50.0f, 50.0f));
							if (ImGui::IsItemHovered())
							{
								ImGui::BeginTooltip();
								ImGui::Image((void*)textures[i]->GetTextureID(), ImVec2(150, 150));
								ImGui::EndTooltip();
							}

							ImGui::SameLine();
							std::string last_element = textures[i]->GetPath().substr(textures[i]->GetPath().rfind('\\') + 1);
							ImGui::Text(last_element.c_str());
						}
					}

					ImGui::PushItemWidth(175);
					// Display uniforms in material
					Ref<Shader> materialShader = materials[selectedIndex]->GetShader();
					const std::unordered_map<std::string, ShaderUniform>& uniforms = materialShader->GetMaterialUniforms();
					for (auto const& [name, uniform] : uniforms)
					{
						std::string uniformName = name.substr(11);
						switch (uniform.Type)
						{
						case UniformType::FLOAT:
						{	
							float& uniform = materials[selectedIndex]->Get<float>(name);
							ImGui::DragFloat(uniformName.c_str(), &uniform, 0.1f, 0.0f, 1.0f);
						}
						break;
						case UniformType::INT:
						{	
							int& uniform = materials[selectedIndex]->Get<int>(name);
							ImGui::DragInt(uniformName.c_str(), &uniform, 0.1f, 0.0f, 1.0f);
						}
						break;
						case UniformType::FLOAT2:
						{	
							glm::vec2& uniform = materials[selectedIndex]->Get<glm::vec2>(name);
							ImGui::DragFloat2(uniformName.c_str(), glm::value_ptr(uniform), 0.1f, 0.0f, 1.0f);
						}
						break;
						case UniformType::FLOAT3:
						{	
							glm::vec3& uniform = materials[selectedIndex]->Get<glm::vec3>(name);
							ImGui::DragFloat3(uniformName.c_str(), glm::value_ptr(uniform), 0.1f, 0.0f, 1.0f);
						}
						break;
						case UniformType::FLOAT4:
						{	
							glm::vec4& uniform = materials[selectedIndex]->Get<glm::vec4>(name);
							ImGui::DragFloat4(uniformName.c_str(), glm::value_ptr(uniform), 0.1f, 0.0f, 1.0f);
						}
						break;
						case UniformType::BOOL:
						{	
							bool& uniform = materials[selectedIndex]->Get<bool>(name);
							ImGui::Checkbox(uniformName.c_str(), &uniform);
						}
						break;
						default:
							NX_CORE_ASSERT(false, "Unknown type");
						}
					}
					ImGui::PopItemWidth();
				}
				// -----------------------------------------------------------

			}
			else
			{
				selectedIndex = -1;
			}
		}

		ImGui::End();
	}

}