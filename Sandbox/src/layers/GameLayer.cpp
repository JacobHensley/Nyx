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

	m_ModelShader = new Shader("res/shaders/ModelShader.shader");
	m_Model = new Model("res/models/Model.fbx");	

	m_ModelMatrix = glm::mat4(1.0f);
	m_ModelMatrix = glm::translate(m_ModelMatrix, glm::vec3(0.0f, 0.0f, -0.1f));
	m_ModelMatrix = glm::rotate(m_ModelMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	m_ModelMatrix = glm::scale(m_ModelMatrix, glm::vec3(0.5f, 0.5f, 0.5f));

	m_TextureCube = new TextureCube("res/textures/canyon_irradiance.png");

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

	m_SkyboxShader = new Shader("res/shaders/Skybox.shader");
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
	m_FrameBuffer->Clear();
	m_FrameBuffer->Bind();

	DebugRenderer::Begin(*m_Camera);
	
	m_SkyboxShader->Bind();

	m_SkyboxShader->SetUniformMat4("u_InverseVP", glm::inverse(m_Camera->GetViewMatrix()));
	m_SkyboxShader->SetUniform1i("u_SkyboxTexture", 0);

	m_TextureCube->bind();
	m_VertexArray->Bind();
	m_IndexBuffer->Bind();
	m_IndexBuffer->Draw(false);
	
	m_ModelShader->Bind();

	glm::mat4 mvp;
	
	m_ModelShader->SetUniform3f("u_Light.Direction", m_Light->Direction);
	m_ModelShader->SetUniform3f("u_Light.Radiance", m_Light->Radiance);
	m_ModelShader->SetUniform1i("u_LightExponent", m_LightExponent);

	m_ModelShader->SetUniform3f("u_CameraPos", m_Camera->GetPosition());

	for (int i = 0;i < 10;i++)
	{
		glm::mat4 transform;
		if (i < 5) 
			transform = glm::translate(m_ModelMatrix, glm::vec3(i * 2.5f, 0.0f, 0.0f));
		else 
			transform = glm::translate(m_ModelMatrix, glm::vec3((i - 5) * 2.5f, -2.5f, 0.0f));
			
		m_ModelShader->SetUniformMat4("u_ModelMatrix", transform);
		mvp = m_Camera->GetProjectionMatrix() * m_Camera->GetViewMatrix() * transform;
		m_ModelShader->SetUniformMat4("u_MVP", mvp);
		m_Model->Render(*m_ModelShader);
		m_Model->DebugDrawBoundingBox(transform);
	}

	DebugRenderer::End();
	DebugRenderer::Flush();
	m_FrameBuffer->Unbind();
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

	m_FrameBuffer->SetViewPortSize(0, 0, (int)ImGui::GetWindowWidth(), (int)ImGui::GetWindowHeight());
	ImGui::Image((void*)(uint64_t)m_FrameBuffer->GetTexture()->GetTextureID(), ImVec2(ImGui::GetWindowWidth(), ImGui::GetWindowHeight()), ImVec2::ImVec2(0, 1), ImVec2::ImVec2(1, 0));

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