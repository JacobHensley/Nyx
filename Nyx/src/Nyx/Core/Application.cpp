#include "NXpch.h"
#include "Application.h"
#include "Nyx/Graphics/Debug/DebugRenderer.h"
#include "Nyx/Graphics/SceneRenderer.h"
#include "Nyx/Graphics/Renderer.h"

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

namespace Nyx {

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		Log::Init();
		NX_CORE_ASSERT(!s_Instance, "Instance of Application already exists");
		s_Instance = this;

		m_Window = CreateRef<Window>("Nyx Engine", 1280, 720);
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));

		SceneRenderer::Init();
		Renderer::Init();
		DebugRenderer::Init();

		m_LayerStack =  CreateRef<LayerStack>();

		m_ImGUILayer = CreateRef<ImGUILayer>("ImGUILayer");
		PushOverlay(m_ImGUILayer);
	}

	Application::~Application()
	{
	}

	void Application::PushLayer(Ref<Layer> layer)
	{
		m_LayerStack->PushLayer(layer);
	}

	void Application::PopLayer(Ref<Layer> layer)
	{
		m_LayerStack->PopLayer(layer);
	}

	void Application::PushOverlay(Ref<Layer> overlay)
	{
		m_LayerStack->PushOverlay(overlay);
	}

	void Application::PopOverlay(Ref<Layer> overlay)
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
		m_Window->ImGuiRender();
		m_ImGUILayer->End();
	}

	void Application::OnEvent(Event& e)
	{
		m_LayerStack->OnEvent(e);
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

}