#include "EditorLayer.h"

EditorLayer::EditorLayer()
	: Layer("Editor")
{
	Init();
}

void EditorLayer::Init()
{
	m_LightEnvironment = CreateRef<LightEnvironment>();
	m_SceneLight = CreateRef<DirectionalLight>(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, -0.5f, -0.75f));
	m_LightEnvironment->SetDirectionalLight(m_SceneLight);

	m_Skybox = CreateRef<EnvironmentMap>(AssetManager::Load<TextureCube>("assets/textures/canyon_Radiance.png"), AssetManager::Load<TextureCube>("assets/textures/canyon_irradiance.png"));
	m_Camera = CreateRef<Camera>(glm::perspectiveFov(glm::radians(45.0f), 1280.0f, 720.0f, 0.01f, 1000.0f));

	m_Scene = CreateRef<Scene>(m_Camera, m_Skybox, m_LightEnvironment);

	defaultMesh = AssetManager::Load<Mesh>("assets/models/backpack/backpack.fbx");

	CreateObject("Default Object", "assets/models/backpack/backpack.fbx", glm::mat4(1.0f));
}

void EditorLayer::Update()
{
	m_Scene->Update();
	UpdateGizmoMode();
}

void EditorLayer::Render()
{
	m_Scene->Render();
}

void EditorLayer::ImGUIRender()
{
	m_SelectedObject = m_Scene->GetSelectedObject();

	RenderViewport();
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
		if (mousePressed.GetMouseButton() == NX_MOUSE_BUTTON_LEFT && m_MouseOverViewport && !Input::IsKeyPressed(NX_KEY_LEFT_ALT) && !ImGuizmo::IsUsing() && !ImGuizmo::IsOver())
			MousePick();
	}
}

void EditorLayer::CreateObject(const String& name, const String& meshPath, glm::mat4& transform)
{
	AssetHandle meshHandle = AssetManager::Load<Mesh>(meshPath);

	Ref<SceneObject> object = m_Scene->CreateObject(name);

	Ref<MeshComponent> meshComponent = CreateRef<MeshComponent>(meshHandle);
	object->AddComponent(meshComponent);

	Ref<TransformComponent> transformComponent = CreateRef<TransformComponent>(transform);
	object->AddComponent(transformComponent);
}

void EditorLayer::RenderSceneWindow()
{
	ImGui::Begin("Scene");

	std::vector<Ref<SceneObject>>& objects = m_Scene->GetSceneObjects();

	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
	if (ImGui::Button("+", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetTextLineHeightWithSpacing())))
	{
		m_Scene->CreateObject("New Object");
	}
	ImGui::PopStyleVar();

	ImGui::Dummy(ImVec2(0.0f, 1.0f));

	static int selected = -1;
	for (int i = 0; i < objects.size(); i++)
	{
		if (objects[i] == m_SelectedObject)
			selected = i;

		if (ImGui::Selectable((objects[i]->GetObjectName() + "##" + std::to_string(i)).c_str(), selected == i) && selected != i)
		{
			m_SelectedObject = objects[i];
			m_Scene->SetSelectedObject(objects[i]);
			selected = i;
		}

		ImGui::OpenPopupOnItemClick(("ContextMenu##" + std::to_string(i)).c_str(), 1);
		if (ImGui::BeginPopup(("ContextMenu##" + std::to_string(i)).c_str()))
		{
			ImGui::Text(objects[i]->GetObjectName().c_str());
			ImGui::Separator();

			if (ImGui::Selectable("Delete"))
			{
				m_Scene->Remove(objects[i]);
			}

			ImGui::EndPopup();
		}
	}

	if (ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered() || m_SelectedObject == nullptr)
	{
		m_SelectedObject = nullptr;
		m_Scene->SetSelectedObject(nullptr);
		selected = -1;
	}

	ImGui::End();
}

void EditorLayer::RenderPropertiesWindow(Ref<SceneObject> object)
{
	ImGui::Begin("Object Properties");

	if (object)
	{
		const String& name = object->GetObjectName();
		String nameInput = name;
		if (ImGui::InputText("##NameInput", &nameInput, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
		{
			if (nameInput != name)
				object->SetObjectName(nameInput);
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
				Ref<TransformComponent> transformComponent = CreateRef<TransformComponent>(glm::mat4(1.0f));
				object->AddComponent(transformComponent);
			}

			if (ImGui::Selectable("Mesh"))
			{
				Ref<MeshComponent> meshComponent = CreateRef<MeshComponent>(defaultMesh);
				object->AddComponent(meshComponent);
			}

			if (ImGui::Selectable("Material"))
			{
				if (!object->GetComponent<MaterialComponent>())
				{
					Ref<PBRMaterial> material = CreateRef<PBRMaterial>(SceneRenderer::GetPBRShader());
					AssetHandle materialHandle = AssetManager::Insert(material);

					Ref<MaterialComponent> materialComponent = CreateRef<MaterialComponent>(materialHandle);
					object->AddComponent(materialComponent);
				}
			}

			ImGui::EndPopup();
		}

		Ref<TransformComponent> transformComponent = object->GetComponent<TransformComponent>();
		if (transformComponent && ImGui::CollapsingHeader("Transform"))
		{
			glm::mat4& transform = transformComponent->GetTransform();
			glm::vec3 scale;
			glm::quat rotation;
			glm::vec3 translation;
			glm::vec3 skew;
			glm::vec4 perspective;
			glm::decompose(transform, scale, rotation, translation, skew, perspective);

			glm::vec3 euler = glm::degrees(glm::eulerAngles(rotation));

			ImGui::DragFloat3("Translation", glm::value_ptr(translation), 0.1f);
			ImGui::DragFloat3("Scale", glm::value_ptr(scale), 0.1f);
			ImGui::DragFloat3("Rotation", glm::value_ptr(euler), 0.1f);
		}

		Ref<MeshComponent> meshComponent = object->GetComponent<MeshComponent>();
		if (meshComponent && ImGui::CollapsingHeader("Mesh"))
		{
			Ref<Mesh> mesh = meshComponent->GetMesh();
			const String& path = mesh->GetPath();
			String meshInput = path;

			if (ImGui::Button(" ... "))
			{
				String file = OpenFileExplorer("FBX\0*.FBX\0");
				if (file != "")
					mesh->Reload(file);
			}

			ImGui::SameLine();

			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 70);
			if (ImGui::InputText("##MeshInput", &meshInput, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
			{
				if (meshInput != path)
					mesh->Reload(meshInput);
			}
		}

		Ref<MaterialComponent> materialComponent = object->GetComponent<MaterialComponent>();
		if (materialComponent && ImGui::CollapsingHeader("Material"))
		{
			Ref<Material> material = materialComponent->GetMaterial();
		}

	}

	ImGui::End();
}

void EditorLayer::RenderSceneSettingsWindow()
{
	ImGui::Begin("Scene Settings");
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
				String path = OpenFileExplorer("Nyx Scene (*.nyx)\0*.nyx\0");
				if (path != "")
				{
					//Create scene from file and replace current scene with it
				}
			}
			if (ImGui::MenuItem("Save")) 
			{
				if (m_Scene->GetPath().empty())
				{
					String path = SaveFileExplorer("Nyx Scene (*.nyx)\0*.nyx\0");
					if (path != "")
					{
						m_Scene->Save(path);
						m_Scene->SetPath(path);
					}
				} 
				else 
				{
					m_Scene->Save(m_Scene->GetPath());
				}
			}
			if (ImGui::MenuItem("Save As")) 
			{
				String path = SaveFileExplorer("Nyx Scene (*.nyx)\0*.nyx\0");
				if (path != "")
				{
					m_Scene->Save(path);
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

	ImGui::Begin("Editor", &open, flags);

	auto viewportOffset = ImGui::GetCursorPos();

	m_ViewportSize.x = ImGui::GetWindowWidth();
	m_ViewportSize.y = ImGui::GetWindowHeight() - viewportOffset.y;

	m_ViewportPosition.x = ImGui::GetWindowPos().x;
	m_ViewportPosition.y = ImGui::GetWindowPos().y;

	m_ViewportPosition.x += viewportOffset.x;
	m_ViewportPosition.y += viewportOffset.y;

	ImGui::Image((void*)(uint64_t)SceneRenderer::GetFinalBuffer()->GetTexture()->GetTextureID(), ImVec2(m_ViewportSize.x, m_ViewportSize.y), ImVec2::ImVec2(0, 1), ImVec2::ImVec2(1, 0));

	if (m_LastViewportSize != m_ViewportSize)
	{
		SceneRenderer::Resize(m_ViewportSize.x, m_ViewportSize.y);
		m_LastViewportSize = m_ViewportSize;
	}

	m_Camera->SetProjectionMatrix(glm::perspectiveFov(glm::radians(45.0f), (float)m_ViewportSize.x, (float)m_ViewportSize.y, 0.01f, 1000.0f));

	ImGuizmo::SetDrawlist();
	ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, m_ViewportSize.x, m_ViewportSize.y);

	m_MouseOverViewport = ImGui::IsWindowHovered();

	
	if (m_SelectedObject)
	{
		Ref<TransformComponent> transformComponent = m_SelectedObject->GetComponent<TransformComponent>();
		if (transformComponent)
			ImGuizmo::Manipulate(glm::value_ptr(m_Camera->GetViewMatrix()), glm::value_ptr(m_Camera->GetProjectionMatrix()), m_GizmoMode, ImGuizmo::WORLD, glm::value_ptr(transformComponent->m_Transform));
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
	auto [mx, my] = ImGui::GetMousePos();
	mx -= m_ViewportPosition.x;
	my -= m_ViewportPosition.y;

	glm::vec2 maxBounds;
	maxBounds.x = m_ViewportPosition.x + m_ViewportSize.x;
	maxBounds.y = m_ViewportPosition.y + m_ViewportSize.y;

	auto viewportWidth = maxBounds.x - m_ViewportPosition.x;
	auto viewportHeight = maxBounds.y - m_ViewportPosition.y;

	glm::vec2 NDC = { (mx / viewportWidth) * 2.0f - 1.0f, ((my / viewportHeight) * 2.0f - 1.0f) * -1.0f };

	glm::vec4 mouseClipPos = { NDC.x, NDC.y, -1.0f, 1.0f };

	auto inverseProj = glm::inverse(m_Scene->GetCamera()->GetProjectionMatrix());
	auto inverseView = glm::inverse(glm::mat3(m_Scene->GetCamera()->GetViewMatrix()));

	auto pos = glm::inverse(m_Scene->GetCamera()->GetViewMatrix())[3];

	glm::vec4 ray = inverseProj * mouseClipPos;
	glm::vec3 rayPos = pos;
	glm::vec3 rayDir = inverseView * glm::vec3(ray);

	m_MouseRay = Ray(rayPos, rayDir);

	SelectObject();
}

bool EditorLayer::SelectObject()
{
	for (Ref<SceneObject> object : m_Scene->GetSceneObjects())
	{
		Ref<Mesh> mesh = object->GetComponent<MeshComponent>()->GetMesh();
		glm::mat4& objectTransform = object->GetComponent<TransformComponent>()->GetTransform();

		for (auto subMesh : mesh->GetSubMeshs())
		{
			glm::mat4 finalTransform = objectTransform * subMesh.transform;

			AABB localBoundingBox = subMesh.boundingBox;
			localBoundingBox.Min = finalTransform * glm::vec4(localBoundingBox.Min, 1.0f);
			localBoundingBox.Max = finalTransform * glm::vec4(localBoundingBox.Max, 1.0f);

			float distance;
			bool boxIntersection = m_MouseRay.IntersectsAABB(localBoundingBox, distance);

			if (boxIntersection)
			{
				Ray localRay = m_MouseRay;
				localRay.Origin = glm::inverse(finalTransform) * glm::vec4(localRay.Origin, 1.0f);
				localRay.Direction = glm::inverse(glm::mat3(finalTransform)) * localRay.Direction;
				
				for (auto& triangle : subMesh.triangles)
				{
					bool triangleIntersection = localRay.IntersectsTriangle(triangle);

					if (triangleIntersection)
					{
						m_SelectedObject = object;
						m_Scene->SetSelectedObject(object);
						return true;
					}
				}
			}
			
		}
		
	}

	m_SelectedObject = nullptr;
	m_Scene->SetSelectedObject(nullptr);
	return false;
	
}
