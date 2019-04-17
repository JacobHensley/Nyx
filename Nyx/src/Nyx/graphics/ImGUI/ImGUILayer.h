#pragma once
#include "NXpch.h"
#include "Nyx/graphics/Layers/Layer.h"

class ImGUILayer : public Layer
{
public:
	ImGUILayer(const String& name);
	~ImGUILayer();

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void ImGUIRender() override;

	void Begin();
	void End();
private:

};