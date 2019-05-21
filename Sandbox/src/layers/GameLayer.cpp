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
	projection = glm::perspectiveFov(glm::radians(45.0f), 1280.0f, 720.0f, 0.01f, 1000.0f);

	glm::mat4 view(1.0f);
	view = glm::translate(view, glm::vec3(0.0f, -0.5f, -4.0f));

	glm::mat4 model(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -0.1f));
	model = glm::rotate(model, glm::radians(m_Angle), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));

	m_ModelShader->SetUniformMat4("u_ModelMatrix", model);

	glm::mat4 mvp = projection * view * model;
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
	ImGui::SliderFloat("Angle", &m_Angle, -360, 360);
}

void GameLayer::OnEvent(Event& e)
{
}