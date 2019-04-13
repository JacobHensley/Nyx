#pragma once
#include "graphics/Window.h"
#include "graphics/Layers/LayerStack.h"

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

	void Run();
private:
	Window* m_Window;
	LayerStack* m_LayerStack;
	bool m_Running;
};