#include "NXpch.h"
#include "Application.h"

/* THIS IS TEST CODE REMOVE AFTER COMMIT */
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

namespace Nyx {

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		NX_CORE_ASSERT(!s_Instance, "Instance of Application already exists");
		s_Instance = this;

		Log::Init();

		m_Window = new Window("Nyx Engine", 1280, 720);
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));

		m_LayerStack = new LayerStack();

		m_ImGUILayer = new ImGUILayer("ImGUILayer");
		PushOverlay(m_ImGUILayer);

		/* THIS IS TEST CODE REMOVE AFTER COMMIT */
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile("", aiProcess_Triangulate | aiProcess_FlipUVs);

		if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			NX_CORE_ASSERT(false, importer.GetErrorString());
		}
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