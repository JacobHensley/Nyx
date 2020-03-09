#include "NXpch.h"
#include "LayerStack.h"

namespace Nyx {

	LayerStack::LayerStack()
	{
	}

	LayerStack::~LayerStack()
	{	
	}

	void LayerStack::PushLayer(Ref<Layer> layer)
	{
		m_Layers.emplace_back(layer);
		layer->OnAttach();
	}

	void LayerStack::PopLayer(Ref<Layer> layer)
	{
		auto it = std::find(m_Layers.begin(), m_Layers.end(), layer);
		if (it != m_Layers.end())
		{
			layer->OnDetach();
			m_Layers.erase(it);
		}
	}

	void LayerStack::PushOverlay(Ref<Layer> overlay)
	{
		m_Layers.emplace_back(overlay);
		overlay->OnAttach();
	}

	void LayerStack::PopOverlay(Ref<Layer> overlay)
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
		for (Ref<Layer> layer : m_Layers)
		{
			layer->Update();
		}
			
		for (Ref<Layer> overlay : m_Overlays)
		{
			overlay->Update();
		}
	}

	void LayerStack::Render()
	{
		for (int i = 0; i < m_Layers.size(); i++)
		{
			if (m_Layers[i]->IsVisible())
			{
				m_Layers[i]->Render();
			}
		}

		for (int i = 0; i < m_Overlays.size(); i++)
		{
			if (m_Overlays[i]->IsVisible())
			{
				m_Overlays[i]->Render();
			}
		}
	}

	void LayerStack::ImGUIRender()
	{
		for (int i = 0; i < m_Layers.size(); i++)
		{
			m_Layers[i]->ImGUIRender();
		}

		for (int i = 0; i < m_Overlays.size(); i++)
		{
			m_Overlays[i]->ImGUIRender();
		}
	}

	void LayerStack::OnEvent(Event& e)
	{
		for (auto it = m_Layers.end(); it != m_Layers.begin(); )
		{
			(*--it)->OnEvent(e);
			if (e.IsHandled())
			{
				break;
			}
		}

		for (auto it = m_Overlays.end(); it != m_Overlays.begin(); )
		{
			(*--it)->OnEvent(e);
			if (e.IsHandled())
			{
				break;
			}	
		}
	}

}