#include "NXpch.h"
#include "Application.h"

Application::Application()
{
	m_Window = new Window("Nyx Engine", 1280, 720);
	Log::Init();
}

Application::~Application()
{
}

void Application::Run()
{
	m_Running = true;

	while (m_Running)
	{
		m_Window->Clear();
		m_Window->Update();
	}
}