#include "NXpch.h"
#include "Input.h"
#include <GLFW/glfw3.h>
#include "Nyx/Application.h"

namespace Nyx {

	Input* Input::s_Instance = new Input();

	bool Input::IsKeyPressed(int keycode)
	{
		GLFWwindow* window = Application::GetApp().GetWindow().GetGLFWWindow();
		return glfwGetKey(window, keycode);
	}

	bool Input::IsMouseButtonPressed(int keycode)
	{
		GLFWwindow* window = Application::GetApp().GetWindow().GetGLFWWindow();
		return glfwGetMouseButton(window, keycode);
	}

	glm::vec2 Input::GetMousePos()
	{
		GLFWwindow* window = Application::GetApp().GetWindow().GetGLFWWindow();
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		return glm::vec2(x, y);
	}

	float Input::GetMouseX()
	{  
		return GetMousePos().x;
	}

	float Input::GetMouseY()
	{
		return GetMousePos().y;
	}

}