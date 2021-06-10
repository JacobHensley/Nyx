#include "ObjectPanel.h"
#include <glm/gtc/type_ptr.hpp>
#include <imgui/misc/cpp/imgui_stdlib.h>

namespace Nyx {

	template<typename T>
	static AssetHandle LoadAssetWidget(Ref<Asset> asset, const std::string& path, const char* filter, const std::string& id)
	{
		std::string meshInput = path;
		meshInput.reserve(128);

		// Load Mesh button
		if (ImGui::Button((" ... " + id).c_str()))
		{
			std::string file = OpenFileExplorer(filter);
			if (file != "")
			{
				if (file != path)
					return AssetManager::Load<T>(file);
				else
					asset->Reload();
			}
		}

		// Mesh path input field
		ImGui::SameLine();
		ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 70);
		if (ImGui::InputText(id.c_str(), &meshInput, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
		{
			if (meshInput != path)
			{
				std::string newString = meshInput.c_str();
				return AssetManager::Load<T>(newString);
			}
		}

		return AssetHandle();
	}

	ObjectPanel::ObjectPanel(Ref<Scene> scene)
		:	m_Scene(scene)
	{
	}

	ObjectPanel::ObjectPanel()
	{
	}

	static int selectedMaterialIndex = -1;
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

			ImGui::SameLine();

			// Button to add component
			if (ImGui::Button("Add Component"))
			{
				ImGui::OpenPopup("AddComponent");
			}

			// Popop to add components
			if (ImGui::BeginPopup("AddComponent"))
			{
				ImGui::Text("Components");
				ImGui::Separator();

				// Add transform component
				if (!selectedObject.HasComponent<TransformComponent>() && ImGui::Selectable("Transform"))
				{
					selectedObject.AddComponent<TransformComponent>();
				}

				// Add mesh component
				if (!selectedObject.HasComponent<MeshComponent>() && ImGui::Selectable("Mesh"))
				{
					auto defaultMesh = AssetManager::Load<Mesh>("assets/models/Cube.fbx");
					selectedObject.AddComponent<MeshComponent>(defaultMesh);
				}

				// Add DirectionalLight component
				if (!selectedObject.HasComponent<DirectionalLightComponent>() && ImGui::Selectable("Directional Light"))
				{
					selectedObject.AddComponent<DirectionalLightComponent>();
				}

				// Add PointLight component
				if (!selectedObject.HasComponent<PointLightComponent>() && ImGui::Selectable("Point Light"))
				{
					selectedObject.AddComponent<PointLightComponent>();
				}

				// Add EnvironmentMap component
				if (!selectedObject.HasComponent<EnvironmentMapComponent>() && ImGui::Selectable("Environment Map"))
				{
					auto radiance = AssetManager::Load<TextureCube>("assets/Textures/Canyon_Radiance.png");
					auto irradiance = AssetManager::Load<TextureCube>("assets/Textures/Canyon_Irradiance.png");
					selectedObject.AddComponent<EnvironmentMapComponent>(radiance, irradiance);
				}

				ImGui::EndPopup();
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
				
				AssetHandle handle = LoadAssetWidget<Mesh>(mesh, mesh->GetPath(), "FBX\0*.FBX\0", "##MeshInput");
				if (handle != 0)
					meshComponent.Mesh = handle;

				// Material list
				std::vector<Ref<Material>> materials = mesh->GetMaterials();
				if (ImGui::ListBoxHeader("##MaterialList", (int)materials.size()))
				{
					for (int i = 0; i < materials.size(); i++)
					{
						if (ImGui::Selectable(materials[i]->GetName().c_str(), i == selectedMaterialIndex))
						{
							selectedMaterialIndex = i;
						}
					}
					ImGui::ListBoxFooter();
					ImGui::Separator();
				}

				// Material viewer
				if (selectedMaterialIndex != -1)
				{
					ImGui::Text(materials[selectedMaterialIndex]->GetShader()->GetPath().c_str());

					// Display all textures in material
					const std::vector<Ref<Texture>>& textures = materials[selectedMaterialIndex]->GetTextures();
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

					// Display uniforms in material
					ImGui::PushItemWidth(175);
					Ref<Shader> materialShader = materials[selectedMaterialIndex]->GetShader();
					const std::unordered_map<std::string, ShaderUniform>& uniforms = materialShader->GetMaterialUniforms();
					for (auto const& [name, uniform] : uniforms)
					{
						std::string uniformName = name.substr(11);
						switch (uniform.Type)
						{
						case UniformType::FLOAT:
						{	
							float& uniform = materials[selectedMaterialIndex]->Get<float>(name);
							ImGui::DragFloat(uniformName.c_str(), &uniform, 0.1f, 0.0f, 1.0f);
						}
						break;
						case UniformType::INT:
						{	
							int& uniform = materials[selectedMaterialIndex]->Get<int>(name);
							ImGui::DragInt(uniformName.c_str(), &uniform, 0.1f, 0, 1);
						}
						break;
						case UniformType::FLOAT2:
						{	
							glm::vec2& uniform = materials[selectedMaterialIndex]->Get<glm::vec2>(name);
							ImGui::DragFloat2(uniformName.c_str(), glm::value_ptr(uniform), 0.1f, 0.0f, 1.0f);
						}
						break;
						case UniformType::FLOAT3:
						{	
							glm::vec3& uniform = materials[selectedMaterialIndex]->Get<glm::vec3>(name);
							ImGui::DragFloat3(uniformName.c_str(), glm::value_ptr(uniform), 0.1f, 0.0f, 1.0f);
						}
						break;
						case UniformType::FLOAT4:
						{	
							glm::vec4& uniform = materials[selectedMaterialIndex]->Get<glm::vec4>(name);
							ImGui::DragFloat4(uniformName.c_str(), glm::value_ptr(uniform), 0.1f, 0.0f, 1.0f);
						}
						break;
						case UniformType::BOOL:
						{	
							bool& uniform = materials[selectedMaterialIndex]->Get<bool>(name);
							ImGui::Checkbox(uniformName.c_str(), &uniform);
						}
						break;
						default:
							NX_CORE_ASSERT(false, "Unknown type");
						}
					}
					ImGui::PopItemWidth();
				}
			}
			else
			{
				selectedMaterialIndex = -1;
			}

			// DirectionalLight component
			if (selectedObject.HasComponent<DirectionalLightComponent>() && ImGui::CollapsingHeader("Directional Light"))
			{
				DirectionalLightComponent& directionalLightComponent = selectedObject.GetComponent<DirectionalLightComponent>();

				ImGui::ColorEdit3("Color", glm::value_ptr(directionalLightComponent.Radiance));

				bool active = (bool)directionalLightComponent.Active;
				if (ImGui::Checkbox("Active##directionalLight", &active))
					directionalLightComponent.Active = (uint32_t)active;
			}

			// PointLight component
			if (selectedObject.HasComponent<PointLightComponent>() && ImGui::CollapsingHeader("Point Light"))
			{
				PointLightComponent& pointLightComponent = selectedObject.GetComponent<PointLightComponent>();

				ImGui::ColorEdit3("Color", glm::value_ptr(pointLightComponent.Radiance));
				ImGui::DragFloat("Intensity", &pointLightComponent.Intensity);

				bool active = (bool)pointLightComponent.Active;
				if (ImGui::Checkbox("Active##pointLight", &active))
					pointLightComponent.Active = (uint32_t)active;
			}

			// EnvironmentMap component
			if (selectedObject.HasComponent<EnvironmentMapComponent>() && ImGui::CollapsingHeader("Environment Map"))
			{
				EnvironmentMapComponent& environmentMapComponent = selectedObject.GetComponent<EnvironmentMapComponent>();

				// Load Radiance Map
				Ref<TextureCube> radianceMap = environmentMapComponent.GetRadianceMap();
				AssetHandle radiancHandle = LoadAssetWidget<TextureCube>(radianceMap, radianceMap->GetPath(), "PNG\0*.PNG\0", "##RadianceInput");
				if (radiancHandle != 0)
					environmentMapComponent.RadianceMap = radiancHandle;

				// Load Irradiance Map
				Ref<TextureCube> irradianceMap = environmentMapComponent.GetIrradianceMap();
				AssetHandle irradianceHandle = LoadAssetWidget<TextureCube>(irradianceMap, irradianceMap->GetPath(), "PNG\0*.PNG\0", "##IrradianceInput");
				if (irradianceHandle != 0)
					environmentMapComponent.IrradianceMap = irradianceHandle;
			}
		}

		ImGui::End();
	}

}