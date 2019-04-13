#pragma once
#include "Nyx.h"

class GameLayer : public Layer
{
public:
	GameLayer(const String& name);

	void OnAttach() override;
	void OnDetach() override;
	void Update() override;
	void Render() override;

private:

};