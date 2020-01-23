#include "NXpch.h"
#include "GraphicsContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <GL/GL.h>
#include "Nyx/GLError.h"
#include "Nyx/scene/SceneRenderer.h"

namespace Nyx {

	GraphicsContext::GraphicsContext(GLFWwindow* window)
		:	m_Window(window)
	{
		NX_CORE_ASSERT(window, "GLFW Window handle is null");
	}

	void GraphicsContext::Init()
	{
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		NX_CORE_ASSERT(status, "Failed to initialize Glad!");

		m_LastTime = (float)glfwGetTime();

		GLErrorInit();

		SetGLLogLevel(GLLogLevel::MEDIUM);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
		glEnable(GL_BLEND);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		SceneRenderer::InitGL();

		NX_CORE_INFO("Created OpenGL Graphics Context");
	}

	void GraphicsContext::SwapBuffers()
	{
		glfwSwapBuffers(m_Window);
	}

	void GraphicsContext::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void GraphicsContext::UpdateFPSCounter()
	{
		float currentTime = (float)glfwGetTime();
		m_FrameCount++;

		if (currentTime - m_LastTime >= 1.0)
		{
			m_MSFrame = (float)(1000.0 / m_FrameCount);
			m_FPS = (float)m_FrameCount;

			if (m_Frames.size() > 100) 
			{
				for (uint i = 1; i < m_Frames.size(); i++)
				{
					m_Frames[i - 1] = m_Frames[i];
				}
				m_Frames[m_Frames.size() - 1] = m_MSFrame;
			}
			else
			{
				m_Frames.push_back(m_MSFrame);
			}

			m_FrameCount = 0;
			m_LastTime += 1.0f;
		}
	}

	void GraphicsContext::RenderImGuiWindow()
	{
		UpdateFPSCounter();

		ImGui::Begin("Graphics Context");

		ImGui::Text("Vendor:   %s", glGetString(GL_VENDOR));
		ImGui::Text("Renderer: %s", glGetString(GL_RENDERER));
		ImGui::Text("Version:  %s", glGetString(GL_VERSION));

		ImGui::PlotLines("ms/f", &m_Frames[0], (int)m_Frames.size(), 0, NULL, 0.0f, 16.0f, ImVec2(0, 80));

		ImGui::End();
	}

}