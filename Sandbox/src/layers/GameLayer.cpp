#include "Nyx.h"
#include "GameLayer.h"

GameLayer::GameLayer(const String& name)
	:	Layer(name) {
}

void GameLayer::OnAttach()
{
	NX_INFO("Attached");
}

void GameLayer::OnDetach()
{
	NX_INFO("Dettached");
}

void GameLayer::Update()
{
	NX_INFO("Update");
}

void GameLayer::Render()
{
	NX_INFO("Render");
}
