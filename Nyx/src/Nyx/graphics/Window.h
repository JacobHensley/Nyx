#pragma once
#include "NXpch.h"
#include "GLFW/glfw3.h"
#include "Nyx/Input/Input.h"

class Window
{
public:
	Window(const String& name, int width, int height);
	~Window();

	void Update();
	void Clear();
	bool IsClosed();

	inline GLFWwindow* GetGLFWWindow() const { return m_Window; }
	inline const int GetWidth() const { return m_Width; }
	inline const int GetHeight() const { return m_Height; }
	inline Input* const GetInput() const { return m_Input; }

private:
	void Init();
	void UpdateFPSCounter();
private:
	GLFWwindow* m_Window;
	const String& m_Name;
	int m_Width, m_Height;
	float m_LastTime, m_MSFrame, m_FPS;
	int m_FrameCount;
private:
	Input* m_Input;
};