#include "Nyx.h"
#include "GameLayer.h"
#include "imgui/imgui.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

using namespace Nyx;

GameLayer::GameLayer(const String& name)
	:	Layer(name) {

	m_VertexArray = new VertexArray();
	m_VertexArray->Bind();

	m_VertexBuffer = new VertexBuffer();
	m_VertexBuffer->Bind();

	float vertices[3 * 3] = {
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.0f,  0.5f, 0.0f
	};

	m_VertexBuffer->SetData(vertices, sizeof(vertices));
	m_VertexBuffer->Bind();

	BufferLayout layout;
	layout.Push<glm::vec3>("pos");
	m_VertexBuffer->SetLayout(layout);

	m_VertexArray->PushVertexBuffer(m_VertexBuffer);
	m_VertexArray->Bind();

	m_IndexBuffer = new IndexBuffer();
	m_IndexBuffer->Bind();

	uint indices[3] = { 0, 1, 2 };
	m_IndexBuffer->SetData(indices, 3);
	m_IndexBuffer->Bind();

	m_Shader = new Shader("res/shaders/Basic.shader");
	m_Shader->Bind();

	m_ModelShader = new Shader("res/shaders/ModelShader.shader");
//	m_ModelShader->Bind();

	m_Model = new Model("res/models/nanosuit.obj");
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
	glm::mat4 projection(1.0f);
	m_ModelShader->SetUniformMat4("projection", projection);

	glm::mat4 view(1.0f);
	m_ModelShader->SetUniformMat4("view", view);

	glm::mat4 model(1.0f);
	model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
	model = glm::translate(model, glm::vec3(transX, transY, transZ));
	m_ModelShader->SetUniformMat4("model", model);

	m_ModelShader->Bind();
//	m_VertexArray->Bind();
//	m_IndexBuffer->Bind();
// 	m_IndexBuffer->Draw();
	
	m_Model->Render(*m_Shader);
}

void GameLayer::ImGUIRender()
{
	
	ImGui::Begin("Test Window");
	ImGui::SliderFloat("X", &transX, -20, 20);
	ImGui::SliderFloat("Y", &transY, -20, 20);
	ImGui::SliderFloat("Z", &transZ, -20, 20);
	ImGui::End();

//	ImGui::Text("Test Text");
}

void GameLayer::OnEvent(Event& e)
{
	NX_TRACE(e);
}
