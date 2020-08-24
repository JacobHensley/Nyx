#include "EditorLayer.h"

EditorLayer::EditorLayer()
	:	Layer("Editor")
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
	
//	CreateObject("Default Object", "assets/models/Sphere1m.fbx", glm::mat4(1.0f));
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
	RenderViewport();
	RenderSceneWindow();
	RenderPropertiesWindow(m_SelectedObject);
	RenderSceneSettingsWindow();
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

	static int selected = -1;

	for (int i = 0; i < objects.size(); i++)
	{
		if (ImGui::Selectable((objects[i]->GetDebugName() + "##" + std::to_string(i)).c_str(), selected == i))
		{
			m_SelectedObject = objects[i];
			selected = i;
		}
			
	}

	if (ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered())
	{
		m_SelectedObject = nullptr;
		selected = -1;
	}

	ImGui::End();
}

void EditorLayer::RenderPropertiesWindow(Ref<SceneObject> object)
{
	ImGui::Begin("Object Properties");

	if (object)
	{
		ImGui::Text(("Name: " + object->GetDebugName()).c_str());
		ImGui::Text("UUID: %ull", object->GetUUID());

		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_None))
		{
			Ref<TransformComponent> transformComponent = object->GetComponent<TransformComponent>();
			glm::mat4& transform = transformComponent->GetTransform();

			glm::vec3 scale;
			glm::quat rotation;
			glm::vec3 translation;
			glm::vec3 skew;
			glm::vec4 perspective;
			glm::decompose(transform, scale, rotation, translation, skew, perspective);

			ImGui::DragFloat3("Translation", glm::value_ptr(translation), 0.1f);
			ImGui::DragFloat3("Scale", glm::value_ptr(scale), 0.1f);


			glm::mat4 newTransform(1.0f);
			newTransform = glm::translate(newTransform, translation);
			newTransform = glm::scale(newTransform, scale);

			transform = newTransform;
		}

		if (ImGui::CollapsingHeader("Model", ImGuiTreeNodeFlags_None))
		{
			Ref<MeshComponent> meshComponent = object->GetComponent<MeshComponent>();
			Ref<Mesh> mesh = meshComponent->GetMesh();
			ImGui::Text(("Path: " + mesh->GetPath()).c_str());
			ImGui::Text("UUID: %i",meshComponent->Get().GetUUID());

			if (ImGui::Button("Load New Mesh"))
			{
				String& path = OpenFileExplorer();
				std::transform(path.begin(), path.end(), path.begin(), ::tolower);
				if (path != "" && path.find(".fbx") != String::npos)
					AssetManager::SwapAsset<Mesh>(meshComponent->Get().GetUUID(), path);
			}

			if (ImGui::Button("Reload Mesh"))
			{
				AssetManager::Load<Mesh>(mesh->GetPath());
			}

			static bool RenderNodeGraph = false;

			if (ImGui::Button("View Node Graph"))
			{
				RenderNodeGraph = !RenderNodeGraph;
			}

			if (RenderNodeGraph)
				mesh->RenderImGuiNodeHierarchy(RenderNodeGraph);

		}

	}

	ImGui::End();
}

void EditorLayer::RenderSceneSettingsWindow()
{
	ImGui::Begin("Scene Settings");

	ImGui::Text("Light");
	ImGui::DragFloat3("Light Direction", glm::value_ptr(m_SceneLight->direction), 0.01f, -1.0f, 1.0f);
	ImGui::DragFloat3("Light Radiance", glm::value_ptr(m_SceneLight->radiance), 0.01f, 0.0f, 1.0f);
	ImGui::Separator();
	ImGui::NewLine();

	ImGui::Text("Camera");
	ImGui::DragFloat("Exposure", m_Camera->GetExposure(), 0.01f, 0.0f, 5.0f);
	ImGui::Checkbox("Enable Exposure", m_Camera->GetExposureActive());
	ImGui::Separator();
	ImGui::NewLine();

	ImGui::Text("Skybox");
/*	if (ImGui::Button("Load New Mesh"))
	{
		String& path = OpenFileExplorer();
		std::transform(path.begin(), path.end(), path.begin(), ::tolower);
		if (path != "" && (path.find(".png") != String::npos || path.find(".tga") != String::npos))
			AssetManager::SwapAsset<TextureCube>(meshComponent->Get().GetUUID(), path);
	} */
	ImGui::Text(AssetManager::GetByUUID<TextureCube>(m_Skybox->radianceMap.GetUUID())->GetPath().c_str());
	ImGui::Separator();
	ImGui::NewLine();

	ImGui::End();
}

void EditorLayer::RenderViewport()
{
	ImGuiIO io = ImGui::GetIO();

	bool open = true;
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
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
	if (m_SelectedObject)
		ImGuizmo::Manipulate(glm::value_ptr(m_Camera->GetViewMatrix()), glm::value_ptr(m_Camera->GetProjectionMatrix()), m_GizmoMode, ImGuizmo::LOCAL, &m_SelectedObject->GetComponent<TransformComponent>()->m_Transform[0][0]);


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
