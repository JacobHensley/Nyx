#pragma once
#include "Nyx/graphics/Window.h"
#include "Nyx/graphics/Layers/LayerStack.h"
#include "Nyx/graphics/ImGUI/ImGUILayer.h"
#include "sol/sol.hpp"

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
		inline static sol::state& GetLuaState() { return s_Instance->m_LuaState; }
		inline static Application& GetApp() { return *s_Instance; }

	private:
		Ref<Window> m_Window;
		Ref<LayerStack> m_LayerStack;
		Ref<ImGUILayer> m_ImGUILayer;
		sol::state m_LuaState;

	private:
		static Application* s_Instance;
	};

}