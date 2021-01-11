#pragma once
#include "Nyx/Core/Core.h"
#include "Nyx/Core/LayerStack.h"
#include "Nyx/Graphics/Window.h"
#include "Nyx/Graphics/ImGUI/ImGUILayer.h"

namespace Nyx {

	class Application
	{
	public:
		Application();
		~Application();

	public:
		void PushLayer(Ref<Layer> layer);
		void PopLayer(Ref<Layer> layer);

		void PushOverlay(Ref<Layer> overlay);
		void PopOverlay(Ref<Layer> overlay);

		void Update();
		void Render();
		void ImGUIRender();

		void OnEvent(Event& e);

		void Run();

		inline Ref<ImGUILayer> GetImGUILayer() { return m_ImGUILayer; }
		inline Ref<Window> GetWindow() { return m_Window; }
		inline static Application& GetApp() { return *s_Instance; }

	private:
		Ref<Window> m_Window;
		Ref<LayerStack> m_LayerStack;
		Ref<ImGUILayer> m_ImGUILayer;

	private:
		static Application* s_Instance;
	};

}