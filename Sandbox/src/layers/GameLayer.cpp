#include "GameLayer.h"
#include "imgui/imgui.h"
#include "imgui/ImGuizmo.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>

#include "Nyx/graphics/DebugRenderer.h"

using namespace Nyx;

GameLayer::GameLayer(const String& name)
	:	Layer(name) {

	m_FrameBuffer = new FrameBuffer(1280, 720, TextureParameters(TextureFormat::RGB, TextureFilter::NEAREST, TextureWrap::CLAMP_TO_EDGE));
	m_FrameBuffer->Bind();

	m_ModelShader = new Shader("assets/shaders/ModelShader.shader");
	m_Model = new Mesh("assets/models/Sphere.fbx");	

	m_ModelMatrix = glm::mat4(1.0f);

	meshComponent = new MeshComponent(*m_Model);
	transformComponent = new TransformComponent(m_ModelMatrix);

	object = SceneObject();
	object.Init(&m_Scene);
	object.AddComponent(new MeshComponent(*m_Model));
	object.AddComponent(new TransformComponent(m_ModelMatrix));
	m_Scene.AddObject(object);


	m_TextureCube = new TextureCube("assets/textures/canyon_irradiance.png");

	m_Light = new Light(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));

	m_Camera = new Camera(glm::perspectiveFov(glm::radians(45.0f), 1280.0f, 720.0f, 0.01f, 1000.0f));

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

	m_VertexBuffer = new VertexBuffer(&vertices[0], (int)(sizeof(QuadVertex) * vertices.size()));

	BufferLayout layout = {
		{ShaderDataType::Vec3, "a_Position"},
		{ShaderDataType::Vec2, "a_TexCoord"}
	};

	m_VertexBuffer->SetLayout(layout);

	m_IndexBuffer = new IndexBuffer(&indices[0], (uint)indices.size());
	m_VertexArray = new VertexArray();
	m_VertexArray->PushVertexBuffer(m_VertexBuffer);

	m_SkyboxShader = new Shader("assets/shaders/Skybox.shader");
}

void GameLayer::OnAttach()
{
}

void GameLayer::OnDetach()
{
}

void GameLayer::Update()
{
	m_Camera->Update();
	m_Scene.Update();
}

void GameLayer::Render()
{
	m_FrameBuffer->Clear();
	m_FrameBuffer->Bind();

	DebugRenderer::Begin(*m_Camera);
	
	m_SkyboxShader->Bind();

	m_SkyboxShader->SetUniformMat4("u_InverseVP", glm::inverse(m_Camera->GetViewMatrix()));
	m_SkyboxShader->SetUniform1i("u_SkyboxTexture", 0);

	m_TextureCube->Bind(0);
	m_VertexArray->Bind();
	m_IndexBuffer->Bind();
	m_IndexBuffer->Draw(false);
	


	m_ModelShader->Bind();
	
	m_ModelShader->SetUniform3f("u_Light.Direction", m_Light->Direction);
	m_ModelShader->SetUniform3f("u_Light.Radiance", m_Light->Radiance);
	m_ModelShader->SetUniform1i("u_LightExponent", m_LightExponent);

	m_ModelShader->SetUniform3f("u_CameraPos", m_Camera->GetPosition());
	m_ModelShader->SetUniformMat4("u_ModelMatrix", m_ModelMatrix);

	glm::mat4 mvp = m_Camera->GetProjectionMatrix() * m_Camera->GetViewMatrix() * m_ModelMatrix;
	m_ModelShader->SetUniformMat4("u_MVP", mvp);

	m_Scene.Render();
	
	DebugRenderer::End();
	DebugRenderer::Flush();
	m_FrameBuffer->Unbind();
}

void GameLayer::MousePick()
{

	Application& app = Application::GetApp();
	glm::vec2 mousePos = Input::GetMousePos();
	mousePos = glm::vec2((m_RenderSpacePos.x - mousePos.x) * -1, (m_RenderSpacePos.y - mousePos.y) * -1);

	NX_DEBUG("mousePos X {0} Y {1}", mousePos.x, mousePos.y);

	glm::vec2 NDC = (mousePos / glm::vec2(m_RenderSpaceWidth, m_RenderSpaceHeight)) * 2.0f - 1.0f;
	NDC.x = (mousePos.x / (float)m_RenderSpaceWidth) * 2.0f - 1.0f;
	NDC.y = (1.0f - mousePos.y / (float)m_RenderSpaceHeight) * 2.0f - 1.0f;

	glm::vec3 dir = glm::inverse(m_Camera->GetProjectionMatrix()) * glm::vec4(NDC.x, NDC.y, -1.0f, 1.0f);
	dir.z = -1.0f;
	dir = glm::normalize(glm::inverse(glm::mat3(m_Camera->GetViewMatrix())) * dir);

	glm::vec3 origin = m_Camera->GetPosition();
	m_Ray = Ray(origin, dir);

	/*	Application& app = Application::GetApp();
		glm::vec2 mousePos = Input::GetMousePos();

		glm::vec2 NDC = (mousePos / glm::vec2(app.GetWindow().GetWidth(), app.GetWindow().GetHeight())) * 2.0f - 1.0f;
		NDC.x = (mousePos.x / (float)app.GetWindow().GetWidth()) * 2.0f - 1.0f;
		NDC.y = (1.0f - mousePos.y / (float)app.GetWindow().GetHeight()) * 2.0f - 1.0f;


		glm::vec3 dir = glm::inverse(m_Camera->GetProjectionMatrix()) * glm::vec4(NDC.x, NDC.y, -1.0f, 1.0f);
		dir.z = -1.0f;
		dir = glm::normalize(glm::inverse(glm::mat3(m_Camera->GetViewMatrix())) * dir);

		glm::vec3 origin = m_Camera->GetPosition();
		m_Ray = Ray(origin, dir); */
}
void GameLayer::ImGUIRender()
{
	ImGuiIO& io = ImGui::GetIO();
	
	ImGui::Begin("Light");
	ImGui::SliderFloat3("Light Direction", glm::value_ptr(m_Light->Direction), -1.0f, 1.0f);
	ImGui::SliderInt("Light Exponent", &m_LightExponent, 1, 3);
	ImGui::ColorPicker3("Light Radiance", glm::value_ptr(m_Light->Radiance));
	ImGui::End();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	bool open = true;
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar;
	io.ConfigWindowsMoveFromTitleBarOnly = true;

	ImGui::Begin("RenderSpace", &open, flags);

	m_RenderSpaceWidth = ImGui::GetWindowWidth();
	m_RenderSpaceHeight = ImGui::GetWindowHeight();

	glm::vec2 windowPos = Application::GetApp().GetWindow().GetWindowPos();

	m_RenderSpacePos.x = ImGui::GetCursorScreenPos().x - windowPos.x;
	m_RenderSpacePos.y = ImGui::GetCursorScreenPos().y - windowPos.y;

	m_FrameBuffer->SetViewPortSize(0, 0, (int)ImGui::GetWindowWidth(), (int)ImGui::GetWindowHeight());
	ImGui::Image((void*)(uint64_t)m_FrameBuffer->GetTexture()->GetTextureID(), ImVec2(ImGui::GetWindowWidth(), ImGui::GetWindowHeight()), ImVec2::ImVec2(0, 1), ImVec2::ImVec2(1, 0));

//	ImGuizmo::SetDrawlist();
//	ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowWidth(), ImGui::GetWindowHeight());

	m_Camera->SetProjectionMatrix(glm::perspectiveFov(glm::radians(45.0f), ImGui::GetWindowWidth(), ImGui::GetWindowHeight(), 0.01f, 1000.0f));

//	ImGuizmo::Manipulate(glm::value_ptr(m_Camera->GetViewMatrix()), glm::value_ptr(m_Camera->GetProjectionMatrix()), ImGuizmo::ROTATE, ImGuizmo::LOCAL, &m_ModelMatrix[0][0]);

	ImGui::End();
	ImGui::PopStyleVar();
}

void GameLayer::OnEvent(Event& e)
{
}