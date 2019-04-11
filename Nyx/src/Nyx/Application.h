#pragma once
#include "graphics/Window.h"

class Application
{
public:
	Application();
	~Application();

	void Run();
private:
	Window* m_Window;
	bool m_Running;
};