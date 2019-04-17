#pragma once
#include "graphics/Window.h"
#include "graphics/Layers/LayerStack.h"
#include "graphics/ImGUI/ImGUILayer.h"

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

	void Run();

	inline Window& GetWindow() { return *m_Window; }
	inline static Application& GetApp() { return *s_Instance; }
private:
	Window* m_Window;
	LayerStack* m_LayerStack;
	ImGUILayer* m_ImGUILayer;
private:
	static Application* s_Instance;
};