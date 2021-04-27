#include "EditorLayer.h"
#include "Nyx/scene/SceneSerializer.h"
#include "Nyx/Asset/AssetManager.h"
#include "Nyx/scene/Components.h"
#include "Nyx/Events/MouseEvent.h"
#include "Nyx/Input/Input.h"
#include "Nyx/Input/KeyCodes.h"
#include "Nyx/Math/Math.h"
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

EditorLayer::EditorLayer()
	: Layer("Editor")
{
	Init();
}

void EditorLayer::Init()
{
	m_EditorCamera = CreateRef<Camera>(glm::perspectiveFov(glm::radians(45.0f), 1280.0f, 720.0f, 0.01f, 1000.0f));
	defaultMesh = AssetManager::Load<Mesh>("assets/models/Cube.fbx");
	m_Scene = SceneSerializer::Load("assets/scenes/Shapes.nyx");

	m_Viewport = ::ViewportPanel();
}

void EditorLayer::Update()
{
	m_Scene->Update();
	m_EditorCamera->Update();
	UpdateGizmoMode();
}

void EditorLayer::Render()
{
	m_Scene->Render(m_EditorCamera);
}

void EditorLayer::ImGUIRender()
{
	m_SelectedObject = m_Scene->GetSelectedObject();

	m_Viewport.SetGizmoObject(m_SelectedObject);
	m_Viewport.Draw(m_EditorCamera);

//	RenderViewport();
	RenderSceneWindow();
	RenderPropertiesWindow(m_SelectedObject);
	RenderSceneSettingsWindow();
	RenderMainMenu();
}

void EditorLayer::OnEvent(Event& e)
{
	if (e.GetEventType() == EventType::MouseButtonPressed)
	{
		MouseButtonEvent& mousePressed = static_cast<MouseButtonEvent&>(e);
		if (mousePressed.GetMouseButton() == NX_MOUSE_BUTTON_LEFT && m_Viewport.IsHovered() && !Input::IsKeyPressed(NX_KEY_LEFT_ALT) && !ImGuizmo::IsUsing() && !ImGuizmo::IsOver())
			MousePick();
	}
}

void EditorLayer::CreateObject(const std::string& name, const std::string& meshPath, glm::mat4& transform)
{
	SceneObject object = m_Scene->CreateObject(name);
	AssetHandle meshHandle = AssetManager::Load<Mesh>(meshPath);
	object.AddComponent<MeshComponent>(meshHandle);
}

void EditorLayer::RenderSceneWindow()
{
	ImGui::Begin("Scene");

	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
	if (ImGui::Button("+", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetTextLineHeightWithSpacing())))
	{
		m_Scene->CreateObject("New Object");
	}
	ImGui::PopStyleVar();

	ImGui::Dummy(ImVec2(0.0f, 1.0f));

	static int selected = -1;
	int i = 0;
	m_Scene->GetRegistry().each([&](auto objectID)
	{
		SceneObject object = { objectID, m_Scene.get() };
		if (!object)
			return;

		if (object == m_SelectedObject)
			selected = i;

		if (ImGui::Selectable((object.GetObjectName() + "##" + std::to_string(i)).c_str(), selected == i) && selected != i)
		{
			m_SelectedObject = object;
			m_Scene->SetSelectedObject(object);
			selected = i;
		}

		ImGui::OpenPopupOnItemClick(("ContextMenu##" + std::to_string(i)).c_str(), 1);
		if (ImGui::BeginPopup(("ContextMenu##" + std::to_string(i)).c_str()))
		{
			ImGui::Text(object.GetObjectName().c_str());
			ImGui::Separator();

			if (ImGui::Selectable("Delete"))
			{
				m_Scene->Remove(object);
				if (m_SelectedObject == object)
					m_SelectedObject = {};
			}

			ImGui::EndPopup();
		}
		i++;
	});


	if (ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered())
	{
		m_Scene->SetSelectedObject(entt::null);
		selected = -1;
	}

	ImGui::End();
}

void EditorLayer::RenderPropertiesWindow(SceneObject object)
{
	ImGui::Begin("Object Properties");

	if (object)
	{
		std::string name = "Untitled Object";
		if (object.HasComponent<TagComponent>())
		{
			name = object.GetComponent<TagComponent>();
		}
		
		std::string nameInput = name;
		if (ImGui::InputText("##NameInput", &nameInput, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
		{
			if (nameInput != name && object.HasComponent<TagComponent>())
				object.GetComponent<TagComponent>().Tag = nameInput;
		}

		ImGui::SameLine();

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
				object.AddComponent<TransformComponent>();
			}

			if (ImGui::Selectable("Mesh"))
			{
				object.AddComponent<MeshComponent>(defaultMesh);
			}

			if (ImGui::Selectable("Material"))
			{
				if (!object.HasComponent<MaterialComponent>())
				{
					Ref<Material> material = CreateRef<Material>(SceneRenderer::GetPBRShader());
					AssetHandle materialHandle = AssetManager::Insert(material);

					object.AddComponent<MaterialComponent>(materialHandle);
				}
			}

			ImGui::EndPopup();
		}

		if (object.HasComponent<TransformComponent>() && ImGui::CollapsingHeader("Transform"))
		{
			TransformComponent& transformComponent = object.GetComponent<TransformComponent>();

			ImGui::DragFloat3("Translation", glm::value_ptr(transformComponent.Translation), 0.1f);
			ImGui::DragFloat3("Scale", glm::value_ptr(transformComponent.Scale), 0.1f);
			ImGui::DragFloat3("Rotation", glm::value_ptr(transformComponent.Rotation), 0.1f);
		}

		if (object.HasComponent<MeshComponent>() && ImGui::CollapsingHeader("Mesh"))
		{
			MeshComponent& meshComponent = object.GetComponent<MeshComponent>();

			Ref<Mesh> mesh = meshComponent.GetMesh();
			const std::string& path = mesh->GetPath();
			std::string meshInput = path;
			meshInput.reserve(128);

			if (ImGui::Button(" ... "))
			{
				std::string file = OpenFileExplorer("FBX\0*.FBX\0");
				if (file != "")
				{
				/*	if (file != path)
						meshComponent.Mesh = AssetManager::Load<Mesh>(file);
					else
						mesh->Reload(file); */
				}
			}

			ImGui::SameLine();

			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 70);
			if (ImGui::InputText("##MeshInput", &meshInput, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
			{
				if (meshInput != path)
				{
					std::string newString = meshInput.c_str();
					meshComponent.Mesh = AssetManager::Load<Mesh>(newString);
				}
			}
		}

		if (object.HasComponent<MaterialComponent>() && ImGui::CollapsingHeader("Material"))
		{
			MaterialComponent& materialComponent = object.GetComponent<MaterialComponent>();
			Ref<Material> material = materialComponent.GetMaterial();
		}

	}

	ImGui::End();
}

void EditorLayer::RenderSceneSettingsWindow()
{
	ImGui::Begin("Scene Settings");
	ImGui::SliderFloat3("Point Light", glm::value_ptr(m_Scene->GetLightEnvironment()->GetPointLight().Position), -1, 1);
	ImGui::SliderFloat3("Dir Light", glm::value_ptr(m_Scene->GetLightEnvironment()->GetDirectionalLight().Direction), -1, 1);
	ImGui::End();
}

void EditorLayer::RenderMainMenu()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New")) 
			{
				//First check if scene has been saved
				//Create new scene with no path
			}
			if (ImGui::MenuItem("Open")) 
			{
				//First check if scene has been saved
				std::string path = OpenFileExplorer("Nyx Scene (*.nyx)\0*.nyx\0");
				if (path != "")
				{
					m_Scene = SceneSerializer::Load(path);
				}
			}
			if (ImGui::MenuItem("Save")) 
			{
				if (m_Scene->GetPath().empty())
				{
					std::string path = SaveFileExplorer("Nyx Scene (*.nyx)\0*.nyx\0");
					if (path != "")
					{
						SceneSerializer::Save(m_Scene, path);
						m_Scene->SetPath(path);
					}
				} 
				else 
				{
					SceneSerializer::Save(m_Scene, m_Scene->GetPath());
				}
			}
			if (ImGui::MenuItem("Save As")) 
			{
				std::string path = SaveFileExplorer("Nyx Scene (*.nyx)\0*.nyx\0");
				if (path != "")
				{
					SceneSerializer::Save(m_Scene, path);
				}
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

void EditorLayer::RenderViewport()
{
	ImGuiIO io = ImGui::GetIO();

	bool open = true;
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse;
	io.ConfigWindowsMoveFromTitleBarOnly = true;
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

	ImGui::Begin("Editor OLD", &open, flags);

	auto viewportOffset = ImGui::GetCursorPos();

	m_ViewportSize.x = ImGui::GetWindowWidth();
	m_ViewportSize.y = ImGui::GetWindowHeight() - viewportOffset.y;

	auto viewportSize = ImGui::GetContentRegionAvail();
	m_ViewportSize = { viewportSize.x, viewportSize.y };

	m_ViewportPosition.x = ImGui::GetWindowPos().x;
	m_ViewportPosition.y = ImGui::GetWindowPos().y;

	m_ViewportPosition.x += viewportOffset.x;
	m_ViewportPosition.y += viewportOffset.y;

	ImGui::Image((void*)(uint64_t)SceneRenderer::GetFinalBuffer()->GetColorAttachments()[0], ImVec2(m_ViewportSize.x, m_ViewportSize.y), ImVec2::ImVec2(0, 1), ImVec2::ImVec2(1, 0));

	auto camera = m_EditorCamera;

	if (m_LastViewportSize != m_ViewportSize)
	{
		SceneRenderer::Resize(m_ViewportSize.x, m_ViewportSize.y);
		m_LastViewportSize = m_ViewportSize;
		camera->SetProjectionMatrix(glm::perspectiveFov(glm::radians(45.0f), (float)m_ViewportSize.x, (float)m_ViewportSize.y, 0.01f, 1000.0f));
	}

	ImGuizmo::SetOrthographic(false);
	ImGuizmo::SetDrawlist();
	float rw = (float)ImGui::GetWindowWidth();
	float rh = (float)ImGui::GetWindowHeight();
	ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y + viewportOffset.y, rw, rh - viewportOffset.y);

	m_MouseOverViewport = ImGui::IsWindowHovered();

	if (m_SelectedObject)
	{
		float rw = (float)ImGui::GetWindowWidth();
		float rh = (float)ImGui::GetWindowHeight();
		
		if (m_SelectedObject.HasComponent<TransformComponent>())
		{
			TransformComponent& transformComponent = m_SelectedObject.GetComponent<TransformComponent>();
			glm::mat4& transform = transformComponent.GetTransform();
			ImGuizmo::Manipulate(glm::value_ptr(camera->GetViewMatrix()), glm::value_ptr(camera->GetProjectionMatrix()), m_GizmoMode, ImGuizmo::LOCAL, glm::value_ptr(transform));
			
			glm::vec3 translation;
			glm::vec3 rotation;
			glm::vec3 scale;
			DecomposeTransform(transform, translation, rotation, scale);
			transformComponent.Translation = translation;
			transformComponent.Rotation = rotation;
			transformComponent.Scale = scale;
		}
			
	}
		

	ImGui::End();
	ImGui::PopStyleVar();
}

void EditorLayer::UpdateGizmoMode()
{
	if (Input::IsKeyPressed(NX_KEY_G))
		m_GizmoMode = ImGuizmo::OPERATION::TRANSLATE;
	if (Input::IsKeyPressed(NX_KEY_R))
		m_GizmoMode = ImGuizmo::OPERATION::ROTATE;
	if (Input::IsKeyPressed(NX_KEY_S))
		m_GizmoMode = ImGuizmo::OPERATION::SCALE;
}

void EditorLayer::MousePick()
{
	glm::vec2 NDC = m_Viewport.GetNDC();

	glm::vec4 mouseClipPos = { NDC.x, NDC.y, -1.0f, 1.0f };

	auto inverseProj = glm::inverse(m_EditorCamera->GetProjectionMatrix());
	auto inverseView = glm::inverse(glm::mat3(m_EditorCamera->GetViewMatrix()));

	auto pos = glm::inverse(m_EditorCamera->GetViewMatrix())[3];

	glm::vec4 ray = inverseProj * mouseClipPos;
	glm::vec3 rayPos = pos;
	glm::vec3 rayDir = inverseView * glm::vec3(ray);

	m_MouseRay = Ray(rayPos, rayDir);

	SelectObject();
}

bool EditorLayer::SelectObject()
{
	bool result = false;

	m_Scene->GetRegistry().each([&](auto objectID)
	{
		SceneObject object = { objectID, m_Scene.get() };
		if (!object)
			return;

		if (!object.HasComponent<MeshComponent>())
			return;

		MeshComponent meshComponent = object.GetComponent<MeshComponent>();

		Ref<Mesh> mesh = meshComponent.GetMesh();
		glm::mat4& objectTransform = object.GetComponent<TransformComponent>().GetTransform();

		for (auto subMesh : mesh->GetSubMeshs())
		{
			glm::mat4 finalTransform = objectTransform * subMesh.Transform;

			AABB localBoundingBox = subMesh.BoundingBox;
			localBoundingBox.Min = finalTransform * glm::vec4(localBoundingBox.Min, 1.0f);
			localBoundingBox.Max = finalTransform * glm::vec4(localBoundingBox.Max, 1.0f);

			float distance;
			bool boxIntersection = m_MouseRay.IntersectsAABB(localBoundingBox, distance);

			if (boxIntersection)
			{
				Ray localRay = m_MouseRay;
				localRay.Origin = glm::inverse(finalTransform) * glm::vec4(localRay.Origin, 1.0f);
				localRay.Direction = glm::inverse(glm::mat3(finalTransform)) * localRay.Direction;

			/*	for (auto& triangle : subMesh.triangles)
				{
					bool triangleIntersection = localRay.IntersectsTriangle(triangle);

					if (triangleIntersection)
					{
						m_SelectedObject = object;
						m_Scene->SetSelectedObject(object);
						result = true;
					} 
				}*/
			}

		}
	});

	if (!result)
	{
		m_Scene->SetSelectedObject(entt::null);
	}

	return result;
}
