#include "NXpch.h"
#include "Window.h"
#include "GLFW/glfw3.h"
#include "glad/glad.h"

Window::Window(const String& name, int width, int height)
	: m_Name(name), m_Width(width), m_Height(height) 
{
	Init();
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

	glClearColor(0.2f, 0.3f, 0.8f, 1.0f);

	glfwSetWindowUserPointer(m_Window, this);
	glfwSwapInterval(0);
}

void Window::Update()
{
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