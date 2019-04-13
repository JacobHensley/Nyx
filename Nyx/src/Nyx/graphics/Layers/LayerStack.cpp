#include "NXpch.h"
#include "LayerStack.h"

LayerStack::LayerStack()
{
}

LayerStack::~LayerStack()
{
	for (Layer* layer : m_Layers)
		delete layer;

	for (Layer* overlay : m_Overlays)
		delete overlay;
}

void LayerStack::PushLayer(Layer* layer)
{
	m_Layers.emplace_back(layer);
	layer->OnAttach();
}

void LayerStack::PopLayer(Layer* layer)
{
	auto it = std::find(m_Layers.begin(), m_Layers.end(), layer);
	if (it != m_Layers.end())
	{
		layer->OnDetach();
		m_Layers.erase(it);
	}
}

void LayerStack::PushOverlay(Layer* overlay)
{
	m_Layers.emplace_back(overlay);
	overlay->OnAttach();
}

void LayerStack::PopOverlay(Layer* overlay)
{
	auto it = std::find(m_Overlays.begin(), m_Overlays.end(), overlay);
	if (it != m_Overlays.end())
	{
		overlay->OnDetach();
		m_Overlays.erase(it);
	}
}

void LayerStack::Update()
{
	for (Layer* layer : m_Layers)
		layer->Update();

	for (Layer* overlay : m_Overlays)
		overlay->Update();
}

void LayerStack::Render()
{
	for (int i = 0;i < m_Layers.size();i++)
	{
		if (m_Layers[i]->IsVisible())
			m_Layers[i]->Render();
	}
		
	for (int i = 0; i < m_Overlays.size(); i++)
	{
		if (m_Overlays[i]->IsVisible())
			m_Overlays[i]->Render();
	}
}
