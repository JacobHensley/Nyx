#pragma once
#include "Nyx.h"

class GameLayer : public Nyx::Layer
{
public:
	GameLayer(const String& name);

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void Update() override;
	virtual void Render() override;
	virtual void ImGUIRender() override;
	virtual void OnEvent(Nyx::Event& e) override;
private:

};