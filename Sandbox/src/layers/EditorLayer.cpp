#include "EditorLayer.h"

EditorLayer::EditorLayer(const String& name) 
	:	Layer(name)
{
	m_Skybox = CreateRef<TextureCube>("assets/textures/canyon_irradiance.png");
	m_LightEnv = CreateRef<LightEnvironment>();
	m_Camera = CreateRef<Camera>(glm::perspectiveFov(glm::radians(45.0f), 1280.0f, 720.0f, 0.01f, 1000.0f));

	parameters.generateMips = true;

	m_AlbedoMap = CreateRef<Texture>("assets/textures/Cerberus_Albedo.tga", parameters);
	m_MetalnessMap = CreateRef<Texture>("assets/textures/Cerberus_Metalness.tga", parameters);
	m_RoughnessMap = CreateRef<Texture>("assets/textures/Cerberus_Roughness.tga", parameters);
	m_NormalMap = CreateRef<Texture>("assets/textures/Cerberus_Normals.tga", parameters);

	m_BRDFLutTexture = CreateRef<Texture>("assets/textures/Brdf_Lut.png", TextureParameters(TextureFormat::RGB, TextureFilter::NEAREST, TextureWrap::CLAMP_TO_EDGE));
	m_IrradianceTexture = CreateRef<TextureCube>("assets/textures/canyon_irradiance.png");
	m_RadianceTexture = CreateRef<TextureCube>("assets/textures/canyon_Radiance.png");
	m_Light = CreateRef<Light>(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, -0.5f, -0.7f));

	Ref<EnvironmentMap> envMap = CreateRef<EnvironmentMap>(m_RadianceTexture, m_IrradianceTexture);
	m_Scene = CreateRef<Scene>(m_Camera, envMap, m_LightEnv);

	m_DefaultMaterial= CreateRef<PBRMaterial>(SceneRenderer::GetPBRShader());

	m_DefaultMaterial->Set("Direction", m_Light->direction);
	m_DefaultMaterial->Set("Radiance", m_Light->radiance);
	m_DefaultMaterial->Set("UsingIBL", (float)m_UsingIBL);
	m_DefaultMaterial->Set("UsingLighting", (float)m_UsingLighting);

	m_DefaultMaterial->SetMetalness(0.0f, m_MetalnessMap, true);
	m_DefaultMaterial->SetRoughness(0.0f, m_RoughnessMap, true);
	m_DefaultMaterial->SetAlbedo(glm::vec3(), m_AlbedoMap, true);
	m_DefaultMaterial->SetNormalMap(m_NormalMap);


	m_Materials["Default Material"] = m_DefaultMaterial;

	// Application::GetCommandLineArgs()[0]
	CreateObject("Default Object", "assets/models/Cerberus.fbx", "Default Material");
}

EditorLayer::~EditorLayer()
{
}

void EditorLayer::OnAttach()
{
}

void EditorLayer::OnDetach()
{
}

void EditorLayer::Update()
{
	m_Scene->Update();
	m_Camera->Update();
}

void EditorLayer::Render()
{
	m_Scene->Render();
}

void EditorLayer::ImGUIRender()
{
	RenderViewport();
	RenderMenuBar();

	if (m_MaterialMenu)
		RenderMaterialMenu();
	if (m_ObjectMenu)
		RenderObjectMenu();
	if (m_SceneGraphMenu)
		RenderSceneGraphMenu();

	auto& sceneObjects = m_Scene->GetSceneObjects();
	for (auto& object : sceneObjects)
	{
		auto meshComponent = object->GetComponent<MeshComponent>();
		if (meshComponent)
			meshComponent->GetMesh()->OnImGuiRender();
	}

}

void EditorLayer::OnEvent(Event& e)
{
}

void EditorLayer::RenderViewport()
{
	ImGuiIO io = ImGui::GetIO();

	//Settings for the viewer window
	bool open = true;
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
	io.ConfigWindowsMoveFromTitleBarOnly = true;

	//Push the settings and start the viewer window
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::Begin("Editor", &open, flags);

	//Get the size of the ImGui viewer window
	m_RenderSpaceSize.x = ImGui::GetWindowWidth();
	m_RenderSpaceSize.y = ImGui::GetWindowHeight();

	//Render the buffer with ImGui::Image
	ImGui::Image((void*)(uint64_t)SceneRenderer::GetFinalBuffer()->GetTexture()->GetTextureID(), ImVec2(m_RenderSpaceSize.x, m_RenderSpaceSize.y), ImVec2::ImVec2(0, 1), ImVec2::ImVec2(1, 0));

	if (m_LastRenderSpaceSize != m_RenderSpaceSize)
	{
		SceneRenderer::Resize(m_RenderSpaceSize.x, m_RenderSpaceSize.y);
		m_LastRenderSpaceSize = m_RenderSpaceSize;
	}

	//Update the cameras projection matrix
	m_Camera->SetProjectionMatrix(glm::perspectiveFov(glm::radians(45.0f), (float)m_RenderSpaceSize.x, (float)m_RenderSpaceSize.y, 0.01f, 1000.0f));

	//Render ImGuizmo
	ImGuizmo::SetDrawlist();
	ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, m_RenderSpaceSize.x, m_RenderSpaceSize.y);
//	if (m_SelectedObject && m_SelectedObject->IsActive())
//		ImGuizmo::Manipulate(glm::value_ptr(m_Camera->GetViewMatrix()), glm::value_ptr(m_Camera->GetProjectionMatrix()), ImGuizmo::OPERATION::SCALE, ImGuizmo::LOCAL, &m_Scene->GetComponent<TransformComponent>(m_SelectedObject)->m_Transform[0][0]);

	//End viewer window
	ImGui::End();
	ImGui::PopStyleVar();
}

void EditorLayer::RenderMenuBar()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New")) {}
			if (ImGui::MenuItem("Open")) {}
			if (ImGui::MenuItem("Save")) {}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Add"))
		{
			if (ImGui::MenuItem("Object")) { m_ObjectMenu = true; }
			if (ImGui::MenuItem("Material")) { m_MaterialMenu = true; }
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("View"))
		{
			if (ImGui::MenuItem("Graphics Context")) {}
			if (ImGui::MenuItem("Object Viewer")) {}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Skybox")) {}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Material Filter"))
		{
			if (ImGui::MenuItem("Normals")) { SceneRenderer::SetMaterialFilter(MaterialFilter::NORMALS); }
			if (ImGui::MenuItem("Texture Coordinates")) { SceneRenderer::SetMaterialFilter(MaterialFilter::TEXTURE_COORDS); }
			if (ImGui::MenuItem("None")) { SceneRenderer::SetMaterialFilter(MaterialFilter::NONE); }
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

void EditorLayer::RenderMaterialMenu()
{
	ImGui::Begin("Load Material", &m_MaterialMenu);

	ImGui::InputText("Material name", &m_MaterialName);

	ImGui::NewLine();

	//Abedo
	{ 
		if (m_AlbedoMap->GetPath() != "" && m_UseAlbedoMap)
		{
			ImGui::Image((void*)(uint64_t)m_AlbedoMap->GetTextureID(), ImVec2(100.0f, 100.0f));
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Image((void*)(uint64_t)m_AlbedoMap->GetTextureID(), ImVec2(200, 200));
				ImGui::EndTooltip();
			}
		}
		if (ImGui::Button("Load albedo map"))
		{
			const String& path = OpenFileExplorer();
			if (path != "" && path.find(".tga") != String::npos)
				m_AlbedoMap->Reload(path);
		}
		ImGui::Checkbox("Use albedo Map", &m_UseAlbedoMap);
		if (!m_UseAlbedoMap)
			ImGui::ColorEdit3("Albedo", glm::value_ptr(m_AlbedoValue));
		ImGui::Separator();
	}

	ImGui::NewLine();

	//Roughness
	{
		if (m_RoughnessMap->GetPath() != "" && m_UseRoughnessMap)
		{
			ImGui::Image((void*)(uint64_t)m_RoughnessMap->GetTextureID(), ImVec2(100.0f, 100.0f));
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Image((void*)(uint64_t)m_RoughnessMap->GetTextureID(), ImVec2(200, 200));
				ImGui::EndTooltip();
			}
		}
		if (ImGui::Button("Load roughness map"))
		{
			const String& path = OpenFileExplorer();
			if (path != "" && path.find(".tga") != String::npos)
				m_RoughnessMap->Reload(path);
		}
		ImGui::Checkbox("Use roughness Map", &m_UseRoughnessMap);
		if (!m_UseRoughnessMap)
			ImGui::SliderFloat("Roughness", &m_RoughnessValue, 0.0f, 1.0f);
		ImGui::Separator();
	}

	ImGui::NewLine();

	//Metalness
	{
		if (m_MetalnessMap->GetPath() != "" && m_UseMetalnessMap)
		{
			ImGui::Image((void*)(uint64_t)m_MetalnessMap->GetTextureID(), ImVec2(100.0f, 100.0f));
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Image((void*)(uint64_t)m_MetalnessMap->GetTextureID(), ImVec2(200, 200));
				ImGui::EndTooltip();
			}
		}
		if (ImGui::Button("Load metalness map"))
		{
			const String& path = OpenFileExplorer();
			if (path != "" && path.find(".tga") != String::npos)
				m_MetalnessMap->Reload(path);
		}
		ImGui::Checkbox("Use metalness Map", &m_UseMetalnessMap);
		if (!m_UseMetalnessMap)
			ImGui::SliderFloat("Metalness", &m_MetalnessValue, 0.0f, 1.0f);
		ImGui::Separator();
	}

	ImGui::NewLine();

	//Normal
	{
		if (m_NormalMap->GetPath() != "" && m_UseNormalMap)
		{
			ImGui::Image((void*)(uint64_t)m_NormalMap->GetTextureID(), ImVec2(100.0f, 100.0f));
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Image((void*)(uint64_t)m_NormalMap->GetTextureID(), ImVec2(200, 200));
				ImGui::EndTooltip();
			}
		}
		if (ImGui::Button("Load normal map"))
		{
			const String& path = OpenFileExplorer();
			if (path != "" && path.find(".tga") != String::npos)
				m_NormalMap->Reload(path);
		}
		ImGui::Checkbox("Use normal Map", &m_UseNormalMap);
	}

	ImGui::NewLine();

	if (m_MaterialName == "")
		ImGui::Text("Name must not be empty");

	bool invaildTexture = false;
	if (m_UseAlbedoMap && m_AlbedoMap->GetWidth() == 0)
	{
		invaildTexture = true;
		ImGui::Text("Must select valid textures");
	}
	else if (m_UseRoughnessMap && m_RoughnessMap->GetWidth() == 0)
	{
		invaildTexture = true;
		ImGui::Text("Must select valid textures");
	}	
	else if (m_UseMetalnessMap && m_MetalnessMap->GetWidth() == 0)
	{
		invaildTexture = true;
		ImGui::Text("Must select valid textures");
	}	
	else if (m_UseNormalMap && m_NormalMap->GetWidth() == 0)
	{
		invaildTexture = true;
		ImGui::Text("Must select valid texture");
	}

	//Create a material with data then reset
	if (ImGui::Button("Save"))
	{
		if (m_MaterialName != "" && !invaildTexture)
		{
			Ref<PBRMaterial> finalMaterial= CreateRef<PBRMaterial>(SceneRenderer::GetPBRShader());

			finalMaterial->SetAlbedo(m_AlbedoValue, CreateRef<Texture>(*m_AlbedoMap), m_UseAlbedoMap);
			finalMaterial->SetMetalness(m_MetalnessValue, CreateRef<Texture>(*m_MetalnessMap), m_UseMetalnessMap);
			finalMaterial->SetRoughness(m_RoughnessValue, CreateRef<Texture>(*m_RoughnessMap), m_UseRoughnessMap);
			finalMaterial->SetNormal(CreateRef<Texture>(*m_NormalMap), m_UseNormalMap);

			m_DefaultMaterial->Set("Direction", m_Light->direction);
			m_DefaultMaterial->Set("Radiance", m_Light->radiance);
			m_DefaultMaterial->Set("UsingIBL", (float)m_UsingIBL);
			m_DefaultMaterial->Set("UsingLighting", (float)m_UsingLighting);

			finalMaterial->Bind();

			m_Materials[m_MaterialName] = finalMaterial;

			m_MaterialMenu = false;

			m_MaterialName = "";

			m_UseAlbedoMap = true;
			m_AlbedoValue = glm::vec3();
			m_AlbedoMap= CreateRef<Texture>(0, 0, parameters);

			m_UseRoughnessMap = true;
			m_RoughnessValue = 0.0f;
			m_RoughnessMap= CreateRef<Texture>(0, 0, parameters);

			m_UseMetalnessMap = true;
			m_MetalnessValue = 0.0f;
			m_MetalnessMap= CreateRef<Texture>(0, 0, parameters);

			m_UseNormalMap = true;
			m_NormalMap= CreateRef<Texture>(0, 0, parameters);
		}
	}

	ImGui::SameLine();

	//Reset all values
	if (ImGui::Button("Cancel"))
	{
		m_MaterialMenu = false;

		m_MaterialName = "";

		m_UseAlbedoMap = true;
		m_AlbedoValue = glm::vec3();
		m_AlbedoMap= CreateRef<Texture>(0, 0, parameters);

		m_UseRoughnessMap = true;
		m_RoughnessValue = 0.0f;
		m_RoughnessMap= CreateRef<Texture>(0, 0, parameters);

		m_UseMetalnessMap = true;
		m_MetalnessValue = 0.0f;
		m_MetalnessMap = CreateRef<Texture>(0, 0, parameters);

		m_UseNormalMap = true;
		m_NormalMap= CreateRef<Texture>(0, 0, parameters);
	}

	ImGui::End();
}

void EditorLayer::RenderObjectMenu()
{
	ImGui::Begin("Load Object", &m_ObjectMenu);

	ImGui::InputText("Object name", &m_ObjectName);

	ImGui::NewLine();

	ImGui::Checkbox("Mesh", &m_SelectedComponents[0]);
	if (m_SelectedComponents[0]) 
	{
		if (ImGui::Button("Load Mesh"))
		{
			String& path = OpenFileExplorer();
			if (path != "" && path.find(".fbx") != String::npos)
			{
				m_MeshPath = path;
			}
		}
		ImGui::SameLine();
		ImGui::Text(m_MeshPath.c_str());
	}

	ImGui::NewLine();

	ImGui::Checkbox("Transform", &m_SelectedComponents[1]);
	if (m_SelectedComponents[1])
	{
	}

	ImGui::NewLine();

	ImGui::Checkbox("Material", &m_SelectedComponents[2]);
	if (m_SelectedComponents[2])
	{
		std::unordered_map<String, Ref<PBRMaterial>>::iterator it = m_Materials.begin();
		if (ImGui::TreeNode("Materials"))
		{
			int index = 0;
			for (std::pair<String, Ref<PBRMaterial>> material : m_Materials) 
			{
				if (ImGui::Selectable(material.first.c_str(), m_SelectedMaterial == index))
				{
					m_SelectedMaterial = index;
					m_SelectedMaterialName = material.first;
				}
					
				index++;
			}

			ImGui::TreePop();
		}
	}

	ImGui::NewLine();

	ImGui::Checkbox("Script", &m_SelectedComponents[3]);
	if (m_SelectedComponents[3])
	{
		if (ImGui::Button("Load Script"))
		{
			String& path = OpenFileExplorer();
			if (path != "" && path.find(".lua") != String::npos)
			{
				m_ScriptPath = path;
			}
		}
		ImGui::SameLine();
		ImGui::Text(m_ScriptPath.c_str());
	}

	ImGui::NewLine();

	bool error = false;

	if (m_ObjectName == "")
	{
		error = true;
		ImGui::Text("Name must not be empty");
	}
		

	if (m_SelectedComponents[0] && m_MeshPath == "")
	{
		error = true;
		ImGui::Text("Must select vaild mesh");
	}
		

	if (m_SelectedComponents[2] && m_SelectedMaterialName == "")
	{
		error = true;
		ImGui::Text("Must select vaild material");
	}
		

	if (m_SelectedComponents[3] && m_ScriptPath == "")
	{
		error = true;
		ImGui::Text("Must select vaild script");
	}
		

	//Create a SceneObject with data then reset
	if (ImGui::Button("Save"))
	{
		if (m_ObjectName != "" && !error)
		{
			CreateObject(m_ObjectName, m_MeshPath, m_SelectedMaterialName, m_ScriptPath);

			m_ObjectMenu = false;

			m_SelectedMaterialName = "";
			m_SelectedMaterial = -1;
			m_ObjectName = "";
			m_MeshPath = "";
			m_ScriptPath = "";

			m_SelectedComponents[0] = true;
			m_SelectedComponents[1] = true;
			m_SelectedComponents[2] = true;
			m_SelectedComponents[3] = false;
		}
	}

	ImGui::SameLine();

	//Reset all values
	if (ImGui::Button("Cancel"))
	{
		m_ObjectMenu = false;

		m_SelectedMaterialName = "";
		m_SelectedMaterial = -1;
		m_ObjectName = "";
		m_MeshPath = "";
		m_ScriptPath = "";

		m_SelectedComponents[0] = true;
		m_SelectedComponents[1] = true;
		m_SelectedComponents[2] = true;
		m_SelectedComponents[3] = false;
	}

	ImGui::End();
}

void EditorLayer::CreateObject(const std::string& objectName, const std::string& meshPath, const std::string& selectedMaterialName, const std::string& scriptPath)
{
	Ref<SceneObject> object = m_Scene->CreateObject(objectName);

	if (m_SelectedComponents[0])
	{
		Ref<MeshComponent> meshComponent = CreateRef<MeshComponent>(CreateRef<Mesh>(meshPath));
		object->AddComponent(meshComponent);
	}

	if (m_SelectedComponents[1])
	{
		Ref<TransformComponent> transformComponent = CreateRef<TransformComponent>(glm::mat4(1.0f));
		object->AddComponent(transformComponent);
	}

	if (m_SelectedComponents[2])
	{
		Ref<MaterialComponent> materialComponent = CreateRef<MaterialComponent>(m_Materials[selectedMaterialName]);
		object->AddComponent(materialComponent);
	}

	if (m_SelectedComponents[3] && !scriptPath.empty())
	{
		Ref<ScriptComponent> scriptComponent = CreateRef<ScriptComponent>(scriptPath);
		object->AddComponent(scriptComponent);
	}
}

void EditorLayer::RenderSceneGraphMenu()
{
	ImGui::Begin("Object Viewer", &m_SceneGraphMenu);

	std::vector<Ref<SceneObject>>& objects = m_Scene->GetSceneObjects();

	if (ImGui::TreeNodeEx("Objects", ImGuiTreeNodeFlags_DefaultOpen))
	{
		for (int i = 0; i < objects.size(); i++)
		{
			if (ImGui::TreeNode(objects[i]->GetDebugName().c_str()))
			{
				bool active = objects[i]->IsActive();
				ImGui::Checkbox("Visible", &active);
				if (active != objects[i]->IsActive())
					objects[i]->SetActive(active);

				if (ImGui::Button("Select"))
				{
					m_SelectedObject = objects[i];
				}

				if (ImGui::Button("Delete"))
				{
				//	m_Scene->RemoveObject(i);
				}

				if (ImGui::Button("Edit Components"))
				{

				}
				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}

	ImGui::End();
}
