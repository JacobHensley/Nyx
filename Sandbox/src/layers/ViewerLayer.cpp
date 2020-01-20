#include "Nyx/Common.h"
#include "ViewerLayer.h"
#include "imgui/imgui.h"
#include "imgui/ImGuizmo.h"
#include "Nyx/graphics/DebugRenderer.h"
#include "Nyx/graphics/MeshFactory.h"
#include "Nyx/Utilities.h"
#include "Nyx/graphics/renderer/Renderer.h"

ViewerLayer::ViewerLayer(const String& name)
	: Layer(name)
{
	//PBR Values
	m_PBRShader = new Shader("assets/shaders/DefaultPBR.shader");

	m_BRDFLutTexture = new Texture("assets/textures/Brdf_Lut.png", TextureParameters(TextureFormat::RGB, TextureFilter::NEAREST, TextureWrap::CLAMP_TO_EDGE));
	m_AlbedoMap = new Texture("assets/textures/Cerberus_Albedo.tga", TextureParameters(TextureFormat::RGB, TextureFilter::NEAREST, TextureWrap::CLAMP_TO_EDGE));
	m_MetalnessMap = new Texture("assets/textures/Cerberus_Metalness.tga", TextureParameters(TextureFormat::RGB, TextureFilter::NEAREST, TextureWrap::CLAMP_TO_EDGE));
	m_NormalMap = new Texture("assets/textures/Cerberus_Normals.tga", TextureParameters(TextureFormat::RGB, TextureFilter::NEAREST, TextureWrap::CLAMP_TO_EDGE));
	m_RoughnessMap = new Texture("assets/textures/Cerberus_Roughness.tga", TextureParameters(TextureFormat::RGB, TextureFilter::NEAREST, TextureWrap::CLAMP_TO_EDGE));
	m_IrradianceTexture = new TextureCube("assets/textures/canyon_irradiance.png");
	m_RadianceTexture = new TextureCube("assets/textures/canyon_Radiance.png");

	//Scene
	m_Camera = new Camera(glm::perspectiveFov(glm::radians(45.0f), 1280.0f, 720.0f, 0.01f, 1000.0f));
	m_Scene = new Scene(m_Camera, m_IrradianceTexture);

	//Scene Object
	m_ObjectMesh = new Mesh("assets/models/Cerberus.FBX");
	m_ObjectMaterial = new PBRMaterial(m_PBRShader);

	m_ObjectMeshComponent = new MeshComponent(m_ObjectMesh);
	m_ObjectTransformComponent = new TransformComponent(m_ObjectTransform);
	m_ObjectMaterialComponent = new MaterialComponent(m_ObjectMaterial);
	m_SceneObject = m_Scene->CreateObject({ m_ObjectMeshComponent, m_ObjectTransformComponent, m_ObjectMaterialComponent });

	m_ObjectTransformComponent->Scale(glm::vec3(0.05f, 0.05f, 0.05f));
	m_ObjectTransformComponent->Rotate(-90, glm::vec3(1.0f, 0.0f, 0.0f));

	//Grid
	m_GridShader = new Shader("assets/shaders/Grid.shader");
	m_GridMesh = MeshFactory::GenQuad(-5.0f, -5.0f, 0.0f, 10.0f, 10.0f);
	m_GridTransform = glm::rotate(m_GridTransform, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	//Skybox
	m_SkyboxShader = new Shader("assets/shaders/Skybox.shader");

	//HDR pass
	m_HDRShader = new Shader("assets/shaders/HDR.shader");
	m_HDRBuffer = new FrameBuffer(1280, 720, TextureParameters(TextureFormat::RGBA16F, TextureFilter::NEAREST, TextureWrap::CLAMP_TO_EDGE));

	//Fullscreen quad (For skybox and HDR)
	m_FullscreenQuad = MeshFactory::GenQuad(-1.0f, -1.0f, 0.0f, 2.0f, 2.0f);

	//Renderspace
	m_RenderSpaceBuffer = new FrameBuffer(1280, 720);

	//Light
	m_Light = Light(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, -0.5f, -0.7f));
	m_LightColor = m_Light.radiance;
}

ViewerLayer::~ViewerLayer()
{
}

void ViewerLayer::MousePick()
{
	Application& app = Application::GetApp();
	glm::vec2 mousePos = Input::GetMousePos();
	mousePos = glm::vec2((m_RenderSpacePosition.x - mousePos.x) * -1, (m_RenderSpacePosition.y - mousePos.y) * -1);

	glm::vec2 NDC;
	NDC.x = (mousePos.x / (float)m_RenderSpaceSize.x) * 2.0f - 1.0f;
	NDC.y = (1.0f - mousePos.y / (float)m_RenderSpaceSize.y) * 2.0f - 1.0f;

	glm::vec3 dir = glm::inverse(m_Camera->GetProjectionMatrix()) * glm::vec4(NDC.x, NDC.y, -1.0f, 1.0f);
	dir.z = -1.0f;
	dir = glm::normalize(glm::inverse(glm::mat3(m_Camera->GetViewMatrix())) * dir);

	glm::vec3 origin = m_Camera->GetViewMatrix()[3];
	m_MouseRay = Ray(origin, dir);
}

void ViewerLayer::Update()
{
	//Update Scene
	m_Scene->Update();
	m_Camera->Update();

	m_Light.radiance = m_LightColor * m_LightMultiplier;

	//Check for camera reset key
	if (Input::IsKeyPressed(NX_KEY_F))
		m_Camera->ResetCamera();
}

void ViewerLayer::Render()
{
	//Draw debug object
	DebugRenderer::Begin(*m_Camera);
	DebugRenderer::DrawLine(m_MouseRay.Origin, m_MouseRay.Origin + m_MouseRay.Direction * 10000.0f);
	DebugRenderer::End();
	DebugRenderer::Flush();

	//Render Grid
	m_GridShader->Bind();
	m_GridShader->SetUniformMat4("u_MVP", m_Camera->GetProjectionMatrix() * m_Camera->GetViewMatrix() * m_GridTransform);
	m_GridShader->SetUniform1f("u_Scale", m_GridScale);
	m_GridShader->SetUniform1f("u_Resolution", m_GridResolution);
	m_GridMesh->Render(true);

	//Upload scene object data
	m_ObjectMaterial->SetUniform("u_Direction", m_Light.direction);
	m_ObjectMaterial->SetUniform("u_Radiance", m_Light.radiance);
	m_ObjectMaterial->SetUniform("u_UsingIBL", (float)m_UsingIBL);
	m_ObjectMaterial->SetUniform("u_UsingLighting", (float)m_UsingLighting);
	m_ObjectMaterial->SetTexture("u_BRDFLutTexture", m_BRDFLutTexture);
	m_ObjectMaterial->SetTexture("u_IrradianceTexture", m_IrradianceTexture);
	m_ObjectMaterial->SetTexture("u_RadianceTexture", m_RadianceTexture);

	m_ObjectMaterial->SetAlbedo(m_Albedo, m_AlbedoMap, m_UsingAlbedoMap);
	m_ObjectMaterial->SetMetalness(m_Metalness, m_MetalnessMap, m_UsingMetalnessMap);
	m_ObjectMaterial->SetRoughness(m_Roughness, m_RoughnessMap, m_UsingRoughnessMap);
	m_ObjectMaterial->SetNormal(m_NormalMap, m_UsingNormalMap);

	m_Scene->Render();

	m_RenderSpaceBuffer = Renderer::GetFinalBuffer();
}

void ViewerLayer::ImGUIRender()
{	
	ImGuiIO& io = ImGui::GetIO();

	//Begin settings panel
	ImGui::Begin("Settings");

	//Reload shaders
	if (ImGui::Button("Refresh PBR Shader"))
		m_PBRShader->Reload();
	if (ImGui::Button("Refresh Skybox Shader"))
		m_SkyboxShader->Reload();
	if (ImGui::Button("Refresh Grid Shader"))
		m_GridShader->Reload();

	//Light options
	ImGui::ColorEdit3("Light Color", glm::value_ptr(m_LightColor));
	ImGui::SliderFloat("Light Multiplier", &m_LightMultiplier, 0.0f, 10.f);
	ImGui::SliderFloat3("Light Direction", glm::value_ptr(m_Light.direction), -1.0f, 1.0f);

	//Set HDR exposure
	ImGui::SliderFloat("HDR Exposure", &m_Exposure, 0.0f, 10.f);
	Renderer::SetExposure(m_Exposure);

	//ImGuizmo mode
	ImGui::RadioButton("TRANSLATE", &m_GizmoMode, 0); ImGui::SameLine();
	ImGui::RadioButton("ROTATE", &m_GizmoMode, 1); ImGui::SameLine();
	ImGui::RadioButton("SCALE", &m_GizmoMode, 2);

	//Use Albedo Map
	ImGui::Checkbox("Enable Albedo Map", &m_UsingAlbedoMap);
	if (!m_UsingAlbedoMap)
		ImGui::ColorEdit3("Albedo", glm::value_ptr(m_Albedo));

	//Use Metalness Map
	ImGui::Checkbox("Enable Metalness Map", &m_UsingMetalnessMap);
	if (!m_UsingMetalnessMap)
		ImGui::SliderFloat("Metalness", &m_Metalness, 0.0f, 1.0f);

	//Use Roughness Map
	ImGui::Checkbox("Enable Roughness Map", &m_UsingRoughnessMap);
	if (!m_UsingRoughnessMap)
		ImGui::SliderFloat("Roughness", &m_Roughness, 0.0f, 1.0f);

	//Use Normal Map
	ImGui::Checkbox("Enable Normal Map", &m_UsingNormalMap);

	//Select lighting mode
	ImGui::Checkbox("Enable IBL", &m_UsingIBL);
	ImGui::Checkbox("Enable Lighting", &m_UsingLighting);

	//Display albedo map preview
	ImGui::Image((void*)(uint64_t)m_AlbedoMap->GetTextureID(), ImVec2(20.0f, 20.0f));
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::Image((void*)(uint64_t)m_AlbedoMap->GetTextureID(), ImVec2(150, 150));
		ImGui::EndTooltip();
	}

	//Load albedo map
	ImGui::SameLine();
	if (ImGui::Button("Load Albedo Map")) 
	{
		const String& path = OpenFileExplorer();
		if (path != "" && path.find(".tga") != String::npos)
			m_AlbedoMap->Reload(path);
	}

	//Display metalness map preview
	ImGui::Image((void*)(uint64_t)m_MetalnessMap->GetTextureID(), ImVec2(20.0f, 20.0f));
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::Image((void*)(uint64_t)m_MetalnessMap->GetTextureID(), ImVec2(150, 150));
		ImGui::EndTooltip();
	}

	//Load metalness map
	ImGui::SameLine();
	if (ImGui::Button("Load Metalness Map"))
	{
		const String& path = OpenFileExplorer();
		if (path != "" && path.find(".tga") != String::npos)
			m_MetalnessMap->Reload(path);
	}

	//Display roughness map preview
	ImGui::Image((void*)(uint64_t)m_RoughnessMap->GetTextureID(), ImVec2(20.0f, 20.0f));
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::Image((void*)(uint64_t)m_RoughnessMap->GetTextureID(), ImVec2(150, 150));
		ImGui::EndTooltip();
	}

	//Load roughness map
	ImGui::SameLine();
	if (ImGui::Button("Load Roughness Map"))
	{
		const String& path = OpenFileExplorer();
		if (path != "" && path.find(".tga") != String::npos)
			m_RoughnessMap->Reload(path);
	}

	//Load object mesh
	if (ImGui::Button("Load Mesh")) 
	{
		const String& path = OpenFileExplorer();
		if (path != "" && path.find(".fbx") != String::npos)
			m_ObjectMesh->Reload(path);
	}

	//Grid options
	ImGui::InputFloat("Grid scale", &m_GridScale, 0.0f, 1000.0f);
	ImGui::InputFloat("Grid resolution", &m_GridResolution, 0.0f, 1.0f);

	//Display object mesh node tree
	m_ObjectMesh->RenderImGuiNodeTree(false);
	
	//End settings panel
	ImGui::End();

	//Settings for the viewer window
	bool open = true;
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
	io.ConfigWindowsMoveFromTitleBarOnly = true;

	//Push the settings and start the viewer window
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::Begin("Viewer", &open, flags);

	//Get the size of the ImGui viewer window
	m_RenderSpaceSize.x = ImGui::GetWindowWidth();
	m_RenderSpaceSize.y = ImGui::GetWindowHeight();

	//Get the offset of the ImGui viewer window from the GLFW window
	glm::vec2 WindowPosition = Application::GetApp().GetWindow().GetWindowPos();
	m_RenderSpacePosition.x = ImGui::GetCursorScreenPos().x - WindowPosition.x;
	m_RenderSpacePosition.y = ImGui::GetCursorScreenPos().y - WindowPosition.y;

	//Set the viewport of the renderspace buffer to the size of the window
	m_RenderSpaceBuffer->SetViewPortSize(0, 0, (int)m_RenderSpaceSize.x, (int)m_RenderSpaceSize.y);
	
	//Render the buffer with ImGui::Image
	ImGui::Image((void*)(uint64_t)m_RenderSpaceBuffer->GetTexture()->GetTextureID(), ImVec2(m_RenderSpaceSize.x, m_RenderSpaceSize.y), ImVec2::ImVec2(0, 1), ImVec2::ImVec2(1, 0));
	
	//Update the cameras projection matrix
	m_Camera->SetProjectionMatrix(glm::perspectiveFov(glm::radians(45.0f), m_RenderSpaceSize.x, m_RenderSpaceSize.y, 0.01f, 1000.0f));

	//Render ImGuizmo
	ImGuizmo::SetDrawlist();
	ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, m_RenderSpaceSize.x, m_RenderSpaceSize.y);
	ImGuizmo::Manipulate(glm::value_ptr(m_Camera->GetViewMatrix()), glm::value_ptr(m_Camera->GetProjectionMatrix()), (ImGuizmo::OPERATION)m_GizmoMode, ImGuizmo::LOCAL, &m_ObjectTransformComponent->m_Transform[0][0]);

	//End viewer window
	ImGui::End();
	ImGui::PopStyleVar();
}

void ViewerLayer::OnEvent(Event& e)
{
	//Dispatch events
	EventDispatcher eventDispatcher = EventDispatcher(e);
	eventDispatcher.Dispatch<MouseButtonPressedEvent>(std::bind(&ViewerLayer::OnMouseClick, this, std::placeholders::_1));
}

bool ViewerLayer::OnMouseClick(MouseButtonPressedEvent& e)
{
	if (Input::IsKeyPressed(NX_KEY_LEFT_CONTROL) && e.GetMouseButton() == NX_MOUSE_BUTTON_LEFT)
		MousePick();

	return true;
}