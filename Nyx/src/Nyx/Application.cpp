#include "NXpch.h"
#include "Application.h"

Application::Application()
{
	m_Window = new Window("Nyx Engine", 1280, 720);
	m_LayerStack = new LayerStack();
	Log::Init();
}

Application::~Application()
{
}

void Application::PushLayer(Layer* layer)
{
	m_LayerStack->PushLayer(layer);
}

void Application::PopLayer(Layer* layer)
{
	m_LayerStack->PopLayer(layer);
}

void Application::PushOverlay(Layer* overlay)
{
	m_LayerStack->PushOverlay(overlay);
}

void Application::PopOverlay(Layer* overlay)
{
	m_LayerStack->PopOverlay(overlay);
}

void Application::Update()
{
	m_LayerStack->Update();
}

void Application::Render()
{
	m_LayerStack->Render();
}

void Application::Run()
{
	m_Running = true;

	while (m_Running)
	{
		Update();

		m_Window->Clear();

		Render();

		m_Window->Update();
	}
}