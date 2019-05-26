//#include "Nyx.h"
#include "GameLayer.h"
#include "imgui/imgui.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "imgui/ImGuizmo.h"
#include <glm/gtc/type_ptr.hpp>

using namespace Nyx;

GameLayer::GameLayer(const String& name)
	:	Layer(name) {

	m_FrameBuffer = new FrameBuffer(1280, 720, TextureParameters(TextureFormat::RGB, TextureFilter::NEAREST, TextureWrap::CLAMP_TO_EDGE));
	m_FrameBuffer->Bind();

	m_VertexArray = new VertexArray();
	m_VertexArray->Bind();

	m_VertexBuffer = new VertexBuffer();
	m_VertexBuffer->Bind();

	float vertices[3 * 5] = {
		 0.25f, 0.25f, 0.0f,  0.0f, 0.0f,
		 0.75f,  0.25f, 0.0f,  1.0f, 0.0f,
		 0.5f,  0.75f, 0.0f,  0.5f, 1.0f
	};

	m_VertexBuffer->SetData(vertices, sizeof(vertices));
	m_VertexBuffer->Bind();

	BufferLayout layout;
	layout.Push<glm::vec3>("pos");
	layout.Push<glm::vec2>("textureCoords");
	m_VertexBuffer->SetLayout(layout);

	m_VertexArray->PushVertexBuffer(m_VertexBuffer);
	m_VertexArray->Bind();

	m_IndexBuffer = new IndexBuffer();
	m_IndexBuffer->Bind();

	uint indices[3] = { 0, 1, 2 };
	m_IndexBuffer->SetData(indices, 3);
	m_IndexBuffer->Bind();

	m_Shader = new Shader("res/shaders/basic.shader");
	m_Texture = new Texture("res/textures/texture.png", TextureParameters(TextureFormat::RGB, TextureFilter::LINEAR, TextureWrap::CLAMP_TO_EDGE));

	m_ModelShader = new Shader("res/shaders/ModelShader.shader");
	m_Model = new Model("res/models/bunny.obj");	

	glm::mat4 projection = glm::perspectiveFov(glm::radians(45.0f), 1280.0f, 720.0f, 0.01f, 1000.0f);
	cam = new Camera(projection);

	cam->SetPosition(glm::vec3(0.0f, 0.25f, 4.0f));

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -0.1f));
	model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
}

void GameLayer::OnAttach()
{
}

void GameLayer::OnDetach()
{
}

void GameLayer::Update()
{
	cam->Update();
}

void GameLayer::Render()
{
	m_ModelShader->Bind();

	m_ModelShader->SetUniformMat4("u_ModelMatrix", model);

	glm::mat4 mvp = cam->GetProjectionMatrix() * cam->GetViewMatrix() * model;
	m_ModelShader->SetUniformMat4("u_MVP", mvp);

	m_Model->Render(*m_ModelShader);
	
	m_Shader->Bind();
	m_Texture->Bind();
	m_VertexArray->Bind();
	m_IndexBuffer->Bind();
	m_IndexBuffer->Draw();
}

void GameLayer::ImGUIRender()
{
	ImGuiIO& io = ImGui::GetIO();
	
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	bool open = true;
	ImGui::SetNextWindowSize(ImVec2::ImVec2(io.DisplaySize.x, io.DisplaySize.y));
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar;
	if (ImGuizmo::IsOver())
		flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove;
	ImGui::Begin("RenderSpace", &open, flags);
//	NX_CORE_INFO("X ({0}), Y ({1})", ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);
	ImGui::Image((void*)m_FrameBuffer->GetTexture()->GetTextureID(), ImVec2::ImVec2(io.DisplaySize.x, io.DisplaySize.y), ImVec2::ImVec2(0, 1), ImVec2::ImVec2(1, 0));

	ImGuizmo::SetDrawlist();
	ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, io.DisplaySize.x, io.DisplaySize.y);
	ImGuizmo::Manipulate(glm::value_ptr(cam->GetViewMatrix()), glm::value_ptr(cam->GetProjectionMatrix()), ImGuizmo::ROTATE, ImGuizmo::LOCAL, &model[0][0]);

	ImGui::End();
	ImGui::PopStyleVar();
}

void GameLayer::OnEvent(Event& e)
{
}