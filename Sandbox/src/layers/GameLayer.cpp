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

	m_ModelShader = new Shader("res/shaders/ModelShader.shader");
	m_Model = new Model("res/models/Model.fbx");	

	m_Camera = new Camera(glm::perspectiveFov(glm::radians(45.0f), 1280.0f, 720.0f, 0.01f, 1000.0f));

	m_ModelMatrix = glm::mat4(1.0f);
	m_ModelMatrix = glm::translate(m_ModelMatrix, glm::vec3(0.0f, 0.0f, -0.1f));
	m_ModelMatrix = glm::rotate(m_ModelMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	m_ModelMatrix = glm::scale(m_ModelMatrix, glm::vec3(0.5f, 0.5f, 0.5f));

	m_Light = new Light();
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
}

void GameLayer::Render()
{
	m_ModelShader->Bind();
	
	m_FrameBuffer->Clear();
	m_FrameBuffer->Bind();

	glm::mat4 mvp;

	for (int i = 0;i < 5;i++)
	{
		m_ModelShader->SetUniformMat4("u_ModelMatrix", glm::translate(m_ModelMatrix, glm::vec3(i * 2.5f, 0.0f, 0.0f)));
		mvp = m_Camera->GetProjectionMatrix() * m_Camera->GetViewMatrix() * glm::translate(m_ModelMatrix, glm::vec3(i * 2.5f, 0.0f, 0.0f));
		m_ModelShader->SetUniformMat4("u_MVP", mvp);

		m_Model->Render(*m_ModelShader);
	}

	for (int i = 0; i < 5; i++)
	{
		m_ModelShader->SetUniformMat4("u_ModelMatrix", glm::translate(m_ModelMatrix, glm::vec3(i * 2.5f, -2.5f, 0.0f)));
		mvp = m_Camera->GetProjectionMatrix() * m_Camera->GetViewMatrix() * glm::translate(m_ModelMatrix, glm::vec3(i * 2.5f, -2.5f, 0.0f));
		m_ModelShader->SetUniformMat4("u_MVP", mvp);

		m_Model->Render(*m_ModelShader);
	}

	m_FrameBuffer->Unbind();
}

void GameLayer::ImGUIRender()
{
	ImGuiIO& io = ImGui::GetIO();
	
	ImGui::Begin("Light");
	ImGui::SliderFloat3("m_Direction", glm::value_ptr(m_Light->Direction), -1.0f, 1.0f);
	ImGui::ColorPicker3("m_Radiance", glm::value_ptr(m_Light->Radiance));
	ImGui::End();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	bool open = true;
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar;
	io.ConfigWindowsMoveFromTitleBarOnly = true;

	ImGui::Begin("RenderSpace", &open, flags);

	m_FrameBuffer->SetViewPortSize(0, 0, ImGui::GetWindowWidth(), ImGui::GetWindowHeight());
	ImGui::Image((void*)m_FrameBuffer->GetTexture()->GetTextureID(), ImVec2(ImGui::GetWindowWidth(), ImGui::GetWindowHeight()), ImVec2::ImVec2(0, 1), ImVec2::ImVec2(1, 0));

	ImGuizmo::SetDrawlist();
	ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowWidth(), ImGui::GetWindowHeight());

	m_Camera->SetProjectionMatrix(glm::perspectiveFov(glm::radians(45.0f), ImGui::GetWindowWidth(), ImGui::GetWindowHeight(), 0.01f, 1000.0f));

	ImGuizmo::Manipulate(glm::value_ptr(m_Camera->GetViewMatrix()), glm::value_ptr(m_Camera->GetProjectionMatrix()), ImGuizmo::ROTATE, ImGuizmo::LOCAL, &m_ModelMatrix[0][0]);

	ImGui::End();
	ImGui::PopStyleVar();
}

void GameLayer::OnEvent(Event& e)
{
}