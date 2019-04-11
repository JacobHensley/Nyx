#include "NXpch.h"
#include "Application.h"

Application::Application()
{
	Log::Init();
}

Application::~Application()
{
}

void Application::Run()
{
	m_Running = true;

	NX_CORE_DEBUG("Test");
	while (m_Running)
	{
	}
}