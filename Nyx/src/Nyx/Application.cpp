#include "NXpch.h"
#include "Application.h"
#include "Nyx/Input/KeyCodes.h"

Application* Application::s_Instance = nullptr;

Application::Application()
{
	NX_CORE_ASSERT(!s_Instance, "Instance of Application already exists");
	s_Instance = this;

	Log::Init();

	m_Window = new Window("Nyx Engine", 1280, 720);
	m_LayerStack = new LayerStack();

	m_ImGUILayer = new ImGUILayer("ImGUILayer");
	PushOverlay(m_ImGUILayer);
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

void Application::ImGUIRender()
{
	m_ImGUILayer->Begin();
	m_LayerStack->ImGUIRender();
	m_ImGUILayer->End();
}

void Application::Run()
{
	while (!m_Window->IsClosed())
	{
		Update();

		m_Window->Clear();

		Render();
		ImGUIRender();

		m_Window->Update();
	}
}