#include "Nyx.h"
#include "GameLayer.h"
#include "imgui/imgui.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

using namespace Nyx;

GameLayer::GameLayer(const String& name)
	:	Layer(name) {

	m_ModelShader = new Shader("res/shaders/ModelShader.shader");
	m_Model = new Model("res/models/bunny.obj");
}

void GameLayer::OnAttach()
{
}

void GameLayer::OnDetach()
{
}

void GameLayer::Update()
{
}

void GameLayer::Render()
{
	m_ModelShader->Bind();

	glm::mat4 projection(1.0f);
	projection = glm::perspective(65.0f, 1280.0f / 720.0f, 0.01f, 1000.0f);

	glm::mat4 view(1.0f);

	glm::mat4 model(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -0.02f));
	model = glm::rotate(model, glm::radians(m_Angle), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));

	m_ModelShader->SetUniformMat4("u_ModelMatrix", model);

	glm::mat4 mvp = projection * view * model;

	m_ModelShader->SetUniformMat4("u_MVP", mvp);

	m_Model->Render(*m_ModelShader);
}

void GameLayer::ImGUIRender()
{
	ImGui::SliderFloat("Angle", &m_Angle, -360, 360);
}

void GameLayer::OnEvent(Event& e)
{
}