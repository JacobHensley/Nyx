#pragma once
#include "NXpch.h"
#include "Layer.h"

class LayerStack
{
public:
	LayerStack();
	~LayerStack();

	void PushLayer(Layer* layer);
	void PopLayer(Layer* layer);

	void PushOverlay(Layer* overlay);
	void PopOverlay(Layer* overlay);

	inline std::vector<Layer*> GetLayers() const { return m_Layers; }
	inline std::vector<Layer*> GetOverlays() const { return m_Overlays; }

	void Update();
	void Render();
private:
	std::vector<Layer*> m_Layers;
	std::vector<Layer*> m_Overlays;
};   