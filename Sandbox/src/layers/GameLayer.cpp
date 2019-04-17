#include "Nyx.h"
#include "GameLayer.h"
#include "imgui/imgui.h"

GameLayer::GameLayer(const String& name)
	:	Layer(name) {
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
}

void GameLayer::ImGUIRender()
{
	
	ImGui::Begin("Test Window");
	ImGui::Text("Hello World");
	ImGui::End();

	ImGui::Text("Test Text");
}
