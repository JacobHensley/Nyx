#include "ViewerLayer.h"
#include "imgui/imgui.h"

#include "Nyx/graphics/DebugRenderer.h"

ViewerLayer::ViewerLayer(const String& name)
	:	Layer(name)
{
	m_RenderSpaceFrameBuffer = new FrameBuffer(1280, 720);
	
	m_Camera = new Camera(glm::perspectiveFov(glm::radians(45.0f), 1280.0f, 720.0f, 0.01f, 1000.0f));

	//Load Textures
	m_BRDFLutTexture = new Texture("assets/textures/Brdf_Lut.png", TextureParameters(TextureFormat::RGB, TextureFilter::NEAREST, TextureWrap::CLAMP_TO_EDGE));
	m_AlbedoMap = new Texture("assets/textures/Cerberus_Albedo.tga", TextureParameters(TextureFormat::RGB, TextureFilter::NEAREST, TextureWrap::CLAMP_TO_EDGE));
	m_MetalnessMap = new Texture("assets/textures/Cerberus_Metalness.tga", TextureParameters(TextureFormat::RGB, TextureFilter::NEAREST, TextureWrap::CLAMP_TO_EDGE));
	m_NormalMap = new Texture("assets/textures/Cerberus_Normals.tga", TextureParameters(TextureFormat::RGB, TextureFilter::NEAREST, TextureWrap::CLAMP_TO_EDGE));
	m_RoughnessMap = new Texture("assets/textures/Cerberus_Roughness.tga", TextureParameters(TextureFormat::RGB, TextureFilter::NEAREST, TextureWrap::CLAMP_TO_EDGE));

	//Load TextureCubes
	m_IrradianceTexture = new TextureCube("assets/textures/canyon_irradiance.png");
	m_RadianceTexture = new TextureCube("assets/textures/canyon_Radiance.png");

	//Load Models
	m_CerberusMesh = new Mesh("assets/models/Cerberus.FBX");

	//Load Shaders
	m_SkyboxShader = new Shader("assets/shaders/Skybox.shader");
	m_PBRShader = new Shader("assets/shaders/DefaultPBR.shader");
	m_ConstPBRShader = new Shader("assets/shaders/ConstPBR.shader");
	m_GridShader = new Shader("assets/shaders/Grid.shader");

	//Setup quads for skybox and grid
	InitSkyboxQuad();
	InitGridQuad();

	//Init Transforms
	m_CerberusTransform = glm::mat4(1.0f);
	m_GridTransform = glm::mat4(1.0f);

	//Init scene
	m_Scene = new Scene();

	//Setup Sphere components and add object to scene
	m_SphereObject = m_Scene->CreateObject({ new MeshComponent(Mesh("assets/models/Sphere.FBX")) , new TransformComponent(glm::mat4(1.0f)) });

	//Setup Cerberus components and add object to scene
	m_CerberusMeshComponent = new MeshComponent(*m_CerberusMesh);
	m_CerberusTransformComponent = new TransformComponent(m_CerberusTransform);
	m_CerberusObject = m_Scene->CreateObject({ m_CerberusMeshComponent , m_CerberusTransformComponent });

	//Modify transforms
	m_CerberusTransformComponent->Scale(glm::vec3(0.05f, 0.05f, 0.05f));
	m_CerberusTransformComponent->Rotate(-90, glm::vec3(1.0f, 0.0f, 0.0f));
	m_GridTransform = glm::rotate(m_GridTransform, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
}

ViewerLayer::~ViewerLayer()
{
	delete m_RenderSpaceFrameBuffer;
	delete m_Camera;
	delete m_IrradianceTexture;
	delete m_SkyboxShader;
	delete m_SkyboxVertexBuffer;
	delete m_SkyboxVertexArray;
	delete m_SkyboxIndexBuffer;
	delete m_CerberusMesh;
	delete m_PBRShader;
	delete m_AlbedoMap;
	delete m_MetalnessMap;
	delete m_NormalMap;
	delete m_RoughnessMap;
	delete m_Scene;
	delete m_CerberusMeshComponent;
	delete m_CerberusTransformComponent;
}

void ViewerLayer::InitSkyboxQuad()
{
	float x = -1.0f, y = -1.0f;
	float width = 2.0f, height = 2.0f;

	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec2 TexCoord;
	};

	std::array<QuadVertex, 4> vertices;

	vertices[0].Position = { x, y, 0.0f };
	vertices[0].TexCoord = { 0.0f, 0.0f };

	vertices[1].Position = { x + width, y, 0.0f };
	vertices[1].TexCoord = { 1.0f, 0.0f };

	vertices[2].Position = { x + width, y + height, 0.0f };
	vertices[2].TexCoord = { 1.0f, 1.0f };

	vertices[3].Position = { x, y + height, 0.0f };
	vertices[3].TexCoord = { 0.0f, 1.0f };

	std::array<uint32_t, 6> indices = { 0, 1, 2, 2, 3, 0 };

	m_SkyboxVertexBuffer = new VertexBuffer(&vertices[0], (int)(sizeof(QuadVertex) * vertices.size()));

	BufferLayout layout = {
		{ShaderDataType::Vec3, "a_Position"},
		{ShaderDataType::Vec2, "a_TexCoord"}
	};

	m_SkyboxVertexBuffer->SetLayout(layout);

	m_SkyboxIndexBuffer = new IndexBuffer(&indices[0], (uint)indices.size());
	m_SkyboxVertexArray = new VertexArray();
	m_SkyboxVertexArray->PushVertexBuffer(m_SkyboxVertexBuffer);
}

void ViewerLayer::InitGridQuad()
{
	float x = -5.0f, y = -5.0f, z = 0.0f;
	float width = 10.0f, height = 10.0f;

	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec2 TexCoord;
	};

	std::array<QuadVertex, 4> vertices;

	vertices[0].Position = { x, y, z };
	vertices[0].TexCoord = { 0.0f, 0.0f };

	vertices[1].Position = { x + width, y, z };
	vertices[1].TexCoord = { 1.0f, 0.0f };

	vertices[2].Position = { x + width, y + height, z };
	vertices[2].TexCoord = { 1.0f, 1.0f };

	vertices[3].Position = { x, y + height, z };
	vertices[3].TexCoord = { 0.0f, 1.0f };

	std::array<uint32_t, 6> indices = { 0, 1, 2, 2, 3, 0 };

	m_GridVertexBuffer = new VertexBuffer(&vertices[0], (int)(sizeof(QuadVertex) * vertices.size()));

	BufferLayout layout = {
		{ShaderDataType::Vec3, "a_Position"},
		{ShaderDataType::Vec2, "a_TexCoord"}
	};

	m_GridVertexBuffer->SetLayout(layout);

	m_GridIndexBuffer = new IndexBuffer(&indices[0], (uint)indices.size());
	m_GridVertexArray = new VertexArray();
	m_GridVertexArray->PushVertexBuffer(m_GridVertexBuffer);
}

void ViewerLayer::MousePick()
{
	Application& app = Application::GetApp();
	glm::vec2 mousePos = Input::GetMousePos();
	mousePos = glm::vec2((m_RenderSpacePosition.x - mousePos.x) * -1, (m_RenderSpacePosition.y - mousePos.y) * -1);

	glm::vec2 NDC = (mousePos / glm::vec2(m_RenderSpaceWidth, m_RenderSpaceHeight)) * 2.0f - 1.0f;
	NDC.x = (mousePos.x / (float)m_RenderSpaceWidth) * 2.0f - 1.0f;
	NDC.y = (1.0f - mousePos.y / (float)m_RenderSpaceHeight) * 2.0f - 1.0f;

	glm::vec3 dir = glm::inverse(m_Camera->GetProjectionMatrix()) * glm::vec4(NDC.x, NDC.y, -1.0f, 1.0f);
	dir.z = -1.0f;
	dir = glm::normalize(glm::inverse(glm::mat3(m_Camera->GetViewMatrix())) * dir);

	glm::vec3 origin = m_Camera->GetPosition();
	m_MouseRay = Ray(origin, dir);
}

void ViewerLayer::Update()
{
	m_Camera->Update();
	m_Scene->Update();

	if (m_HoldRay && Input::IsMouseButtonPressed(NX_MOUSE_BUTTON_LEFT) && Input::IsKeyPressed(NX_KEY_LEFT_CONTROL))
		MousePick();
}

void ViewerLayer::Render()
{
	m_RenderSpaceFrameBuffer->Clear();
	m_RenderSpaceFrameBuffer->Bind();

	DebugRenderer::Begin(*m_Camera);

	//Render Skybox
	m_SkyboxShader->Bind();

	m_SkyboxShader->SetUniformMat4("u_InverseVP", glm::inverse(m_Camera->GetViewMatrix()));

	m_SkyboxShader->SetUniform1i("u_SkyboxTexture", 0);
	m_IrradianceTexture->Bind(0);

	m_SkyboxVertexArray->Bind();
	m_SkyboxIndexBuffer->Bind();
	m_SkyboxIndexBuffer->Draw(false);

	//Upload uniforms for PBR shader
	m_PBRShader->Bind();

	m_PBRShader->SetUniformMat4("u_ModelMatrix", m_CerberusTransformComponent->m_Transform);
	m_PBRShader->SetUniform3f("u_CameraPosition", m_Camera->GetPosition());
	m_PBRShader->SetUniformMat4("u_MVP", m_Camera->GetProjectionMatrix() * m_Camera->GetViewMatrix() * m_CerberusTransformComponent->m_Transform);

	m_PBRShader->SetUniform1i("u_BRDFLutTexture", 0);
	m_BRDFLutTexture->Bind(0);

	m_PBRShader->SetUniform1i("u_IrradianceTexture", 1);
	m_IrradianceTexture->Bind(1);

	m_PBRShader->SetUniform1i("u_RadianceTexture", 2);
	m_RadianceTexture->Bind(2);

	m_PBRShader->SetUniform1i("u_AlbedoMap", 3);
	m_AlbedoMap->Bind(3);

	m_PBRShader->SetUniform1i("u_MetalnessMap", 4);
	m_MetalnessMap->Bind(4);

	m_PBRShader->SetUniform1i("u_NormalMap", 5);
	m_NormalMap->Bind(5);

	m_PBRShader->SetUniform1i("u_RoughnessMap", 6);
	m_RoughnessMap->Bind(6);

	//Upload uniforms for const PBR shader
	m_ConstPBRShader->Bind();

	m_ConstPBRShader->SetUniformMat4("u_ModelMatrix", m_Scene->GetComponent<TransformComponent>(m_SphereObject)->GetTransform());
	m_ConstPBRShader->SetUniform3f("u_CameraPosition", m_Camera->GetPosition());
	m_ConstPBRShader->SetUniformMat4("u_MVP", m_Camera->GetProjectionMatrix() * m_Camera->GetViewMatrix() * m_Scene->GetComponent<TransformComponent>(m_SphereObject)->GetTransform());

	m_PBRShader->SetUniform1i("u_IrradianceTexture", 0);
	m_IrradianceTexture->Bind(0);

	m_PBRShader->SetUniform1i("u_RadianceTexture", 1);
	m_RadianceTexture->Bind(1);

	m_ConstPBRShader->SetUniform3f("u_Albedo", m_Albedo);
	m_ConstPBRShader->SetUniform1f("u_Metalness", m_Metalness);
	m_ConstPBRShader->SetUniform1f("u_Normal", m_Normal);
	m_ConstPBRShader->SetUniform1f("u_Roughness", m_Roughness);

	if (m_SceneMode == 0) 
	{
		m_SphereObject->SetVisibility(false);
		m_CerberusObject->SetVisibility(true);
		m_PBRShader->Bind();
	}
	else if (m_SceneMode == 1) 
	{
		m_SphereObject->SetVisibility(true);
		m_CerberusObject->SetVisibility(false);
		m_ConstPBRShader->Bind();
	}
		
	//Render Scene
	m_Scene->Render();

	//Render mouse ray
	DebugRenderer::DrawLine(m_MouseRay.Origin, m_MouseRay.Origin + m_MouseRay.Direction * 10000.0f);

	//Render Grid
	m_GridShader->Bind();

	m_GridShader->SetUniformMat4("u_MVP", m_Camera->GetProjectionMatrix() * m_Camera->GetViewMatrix() * m_GridTransform);

	m_GridVertexArray->Bind();
	m_GridIndexBuffer->Bind();
	m_GridIndexBuffer->Draw(true);

	DebugRenderer::End();
	DebugRenderer::Flush();

	m_RenderSpaceFrameBuffer->Unbind();
}

void ViewerLayer::ImGUIRender()
{
	ImGui::Begin("Settings");

	if (ImGui::Button("Refresh PBR Shader"))
		m_PBRShader->Reload();
	if (ImGui::Button("Refresh Skybox Shader"))
		m_SkyboxShader->Reload();
	if (ImGui::Button("Refresh Grid Shader"))
		m_GridShader->Reload();
	if (ImGui::Button("Refresh Const PBR Shader"))
		m_ConstPBRShader->Reload();
	ImGui::Separator();

	ImGui::RadioButton("Cerberus", &m_SceneMode, 0); ImGui::SameLine();
	ImGui::RadioButton("Sphere", &m_SceneMode, 1);
	ImGui::Separator();

	if (m_SceneMode == 1) 
	{
		ImGui::ColorEdit3("Albedo", glm::value_ptr(m_Albedo));
		ImGui::SliderFloat("Metalness", &m_Metalness, 0, 1);
		ImGui::SliderFloat("Normal", &m_Normal, -1, 1);
		ImGui::SliderFloat("Roughness", &m_Roughness, 0, 1);
		ImGui::Separator();
	}

	ImGui::RadioButton("TRANSLATE", &m_GizmoMode, 0); ImGui::SameLine();
	ImGui::RadioButton("ROTATE", &m_GizmoMode, 1); ImGui::SameLine();
	ImGui::RadioButton("SCALE", &m_GizmoMode, 2);
	ImGui::Separator();

	ImGui::Checkbox("Toggle Ray Hold", &m_HoldRay);
	ImGui::Separator();

	ImGui::End();

	ImGuiIO& io = ImGui::GetIO();

	bool open = true;
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar;
	io.ConfigWindowsMoveFromTitleBarOnly = true;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::Begin("Viewer", &open, flags);

	m_RenderSpaceWidth = ImGui::GetWindowWidth();
	m_RenderSpaceHeight = ImGui::GetWindowHeight();

	glm::vec2 WindowPosition = Application::GetApp().GetWindow().GetWindowPos();

	m_RenderSpacePosition.x = ImGui::GetCursorScreenPos().x - WindowPosition.x;
	m_RenderSpacePosition.y = ImGui::GetCursorScreenPos().y - WindowPosition.y;

	m_RenderSpaceFrameBuffer->SetViewPortSize(0, 0, m_RenderSpaceWidth, m_RenderSpaceHeight);
	ImGui::Image((void*)(uint64_t)m_RenderSpaceFrameBuffer->GetTexture()->GetTextureID(), ImVec2(m_RenderSpaceWidth, m_RenderSpaceHeight), ImVec2::ImVec2(0, 1), ImVec2::ImVec2(1, 0));

	ImGuizmo::SetDrawlist();
	ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, m_RenderSpaceWidth, m_RenderSpaceHeight);

	m_Camera->SetProjectionMatrix(glm::perspectiveFov(glm::radians(45.0f), (float)m_RenderSpaceWidth, (float)m_RenderSpaceHeight, 0.01f, 1000.0f));

	if (m_SceneMode == 0)
		ImGuizmo::Manipulate(glm::value_ptr(m_Camera->GetViewMatrix()), glm::value_ptr(m_Camera->GetProjectionMatrix()), (ImGuizmo::OPERATION)m_GizmoMode, ImGuizmo::LOCAL, &m_CerberusTransformComponent->m_Transform[0][0]);
	if (m_SceneMode == 1)
		ImGuizmo::Manipulate(glm::value_ptr(m_Camera->GetViewMatrix()), glm::value_ptr(m_Camera->GetProjectionMatrix()), (ImGuizmo::OPERATION)m_GizmoMode, ImGuizmo::LOCAL, &m_Scene->GetComponent<TransformComponent>(m_SphereObject)->m_Transform[0][0]);

	ImGui::End();
	ImGui::PopStyleVar();
}

void ViewerLayer::OnEvent(Event& e)
{
	EventDispatcher eventDispatcher = EventDispatcher(e);
	eventDispatcher.Dispatch<MouseButtonPressedEvent>(std::bind(&ViewerLayer::OnMouseClick, this, std::placeholders::_1));
}

bool ViewerLayer::OnMouseClick(MouseButtonPressedEvent& e)
{
	if (!m_HoldRay && Input::IsKeyPressed(NX_KEY_LEFT_CONTROL) && e.GetMouseButton() == NX_MOUSE_BUTTON_LEFT)
		MousePick();
	return true;
}
