#pragma once
#include "Nyx.h"

class GameLayer : public Layer
{
public:
	GameLayer(const String& name);

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void Update() override;
	virtual void Render() override;
	virtual void ImGUIRender() override;

private:

};