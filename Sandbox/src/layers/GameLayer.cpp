#include "Nyx.h"
#include "GameLayer.h"
#include "imgui/imgui.h"

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

	m_Shader->SetUniform4f("u_Color", 0.2f, 1.0f, 1.0f, 1.0f);
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
	m_VertexArray->Bind();
	m_IndexBuffer->Bind();
	m_IndexBuffer->Draw();
}

void GameLayer::ImGUIRender()
{
	
	ImGui::Begin("Test Window");
	ImGui::Text("Hello World");
	ImGui::End();

	ImGui::Text("Test Text");
}

void GameLayer::OnEvent(Event& e)
{
	NX_TRACE(e);
}
