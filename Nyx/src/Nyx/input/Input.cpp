#include "NXpch.h"
#include "Input.h"
#include <GLFW/glfw3.h>
#include "Nyx/Application.h"

namespace Nyx {

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

	std::pair<float, float> Input::GetMousePos()
	{
		GLFWwindow* window = Application::GetApp().GetWindow().GetGLFWWindow();
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		return { (float)x, (float)y };
	}

	float Input::GetMouseX()
	{
		auto[x, y] = GetMousePos();
		return x;
	}

	float Input::GetMouseY()
	{
		auto[x, y] = GetMousePos();
		return y;
	}

}