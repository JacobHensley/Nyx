#include "NXpch.h"
#include "Window.h"
#include "GLFW/glfw3.h"
#include "glad/glad.h"
#include "Nyx/GLError.h"
#include "Nyx/Events/AppEvent.h"
#include "Nyx/Events/KeyEvent.h"
#include "Nyx/Events/MouseEvent.h"

Window::Window(const String& name, int width, int height)
	: m_Name(name), m_Width(width), m_Height(height) 
{
	Init();
	m_LastTime = glfwGetTime();
}

Window::~Window()
{
	glfwTerminate();
}

void Window::Init()
{
	NX_ASSERT(glfwInit(), "GLFW Failed to Initialize");

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	m_Window = glfwCreateWindow(m_Width, m_Height, m_Name.c_str(), NULL, NULL);
	glfwMakeContextCurrent(m_Window);

	NX_ASSERT(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress), "Glad Failed to Initialize");

	GLErrorInit();

	glClearColor(0.2f, 0.3f, 0.8f, 1.0f);

	glfwSetWindowUserPointer(m_Window, this);
	glfwSwapInterval(0);

	m_Input = new Input();

	glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
	{
		Window& win = *(Window*)glfwGetWindowUserPointer(window);
		win.m_Width = width;
		win.m_Height = height;

		WindowResizeEvent event(width, height);
		win.m_EventCallback(event);
	});

	glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
	{
		Window& win = *(Window*)glfwGetWindowUserPointer(window);
		WindowCloseEvent event;
		win.m_EventCallback(event);
	});

	glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		Window& win = *(Window*)glfwGetWindowUserPointer(window);
		switch (action)
		{
		case GLFW_PRESS:
		{
			KeyPressedEvent event(key, 0);
			win.m_EventCallback(event);
			break;
		}
		case GLFW_RELEASE:
		{
			KeyReleasedEvent event(key);
			win.m_EventCallback(event);
			break;
		}
		case GLFW_REPEAT:
		{
			KeyPressedEvent event(key, 1);
			win.m_EventCallback(event);
			break;
		}
		}
	});

	glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode)
	{
		Window& win = *(Window*)glfwGetWindowUserPointer(window);
		KeyTypedEvent event(keycode);
		win.m_EventCallback(event);
	});

	glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
	{
		Window& win = *(Window*)glfwGetWindowUserPointer(window);
		switch (action)
		{
		case GLFW_PRESS:
		{
			MouseButtonPressedEvent event(button);
			win.m_EventCallback(event);
			break;
		}
		case GLFW_RELEASE:
		{
			MouseButtonReleasedEvent event(button);
			win.m_EventCallback(event);
			break;
		}
		}
	});

	glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
	{
		Window& win = *(Window*)glfwGetWindowUserPointer(window);
		MouseScrolledEvent event((float)xOffset, (float)yOffset);
		win.m_EventCallback(event);
	});

	glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
	{
		Window& win = *(Window*)glfwGetWindowUserPointer(window);
		MouseMovedEvent event((float)xPos, (float)yPos);
		win.m_EventCallback(event);
	});
}

void Window::UpdateFPSCounter()
{
	float currentTime = (float)glfwGetTime();
	m_FrameCount++;

	if (currentTime - m_LastTime >= 1.0)
	{
		m_MSFrame = 1000.0 / m_FrameCount;
		m_FPS = m_FrameCount;

		m_FrameCount = 0;
		m_LastTime += 1.0f;

		NX_CORE_TRACE(m_MSFrame);
		NX_CORE_TRACE(m_FPS);
	}
}

void Window::Update()
{
	UpdateFPSCounter();
	glfwSwapBuffers(m_Window);
	glfwPollEvents();
}

void Window::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

bool Window::IsClosed()
{
	return glfwWindowShouldClose(m_Window);
}