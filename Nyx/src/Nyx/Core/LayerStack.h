#pragma once
#include "Layer.h"
#include "Nyx/Core/Core.h"
#include "Nyx/Events/Event.h"

namespace Nyx {

	class LayerStack
	{
	public:
		LayerStack();
		~LayerStack();

	public:
		void PushLayer(Ref<Layer> layer);
		void PopLayer(Ref<Layer> layer);

		void PushOverlay(Ref<Layer> overlay);
		void PopOverlay(Ref<Layer> overlay);

		void Update();
		void Render();
		void ImGUIRender();

		inline std::vector<Ref<Layer>>& GetLayers() { return m_Layers; }
		inline std::vector<Ref<Layer>>& GetOverlays() { return m_Overlays; }

		void OnEvent(Event& e);
	private:

		std::vector<Ref<Layer>> m_Layers;
		std::vector<Ref<Layer>> m_Overlays;
	};

}