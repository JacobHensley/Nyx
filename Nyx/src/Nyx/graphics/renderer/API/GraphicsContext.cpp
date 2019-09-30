#include "NXpch.h"
#include "GraphicsContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <GL/GL.h>
#include "Nyx/GLError.h"

namespace Nyx {

	GraphicsContext::GraphicsContext(GLFWwindow* window)
		:	m_Window(window)
	{
		NX_CORE_ASSERT(window, "Window handle is null");
	}

	void GraphicsContext::Init()
	{
		glfwMakeContextCurrent(m_Window);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		NX_CORE_ASSERT(status, "Failed to initialize Glad!");

		GLErrorInit();

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
		glEnable(GL_BLEND);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		NX_CORE_INFO("OpenGL Info:");
		NX_CORE_INFO("	Vendor:   {0}", glGetString(GL_VENDOR));
		NX_CORE_INFO("	Renderer: {0}", glGetString(GL_RENDERER));
		NX_CORE_INFO("	Version:  {0}", glGetString(GL_VERSION));
	}

	void GraphicsContext::SwapBuffers()
	{
		glfwSwapBuffers(m_Window);
	}

	void GraphicsContext::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

}