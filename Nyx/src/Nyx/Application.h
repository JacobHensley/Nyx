#pragma once
#include "Nyx/graphics/Window.h"
#include "Nyx/graphics/Layers/LayerStack.h"
#include "Nyx/graphics/ImGUI/ImGUILayer.h"

namespace Nyx {

	class Application
	{
	public:
		Application();
		~Application();

		void PushLayer(Layer* layer);
		void PopLayer(Layer* layer);

		void PushOverlay(Layer* overlay);
		void PopOverlay(Layer* overlay);

		void Update();
		void Render();
		void ImGUIRender();

		void OnEvent(Event& e);

		void Run();

		inline ImGUILayer& GetImGUILayer() { return *m_ImGUILayer; }
		inline Window& GetWindow() { return *m_Window; }
		inline static Application& GetApp() { return *s_Instance; }

	private:
		Window* m_Window;
		LayerStack* m_LayerStack;
		ImGUILayer* m_ImGUILayer;
	private:
		static Application* s_Instance;
	};

}