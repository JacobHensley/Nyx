#pragma once
#include "NXpch.h"
#include "GLFW/glfw3.h"

class Window
{
public:
	Window(const String& name, int width, int height);
	~Window();

	void Update();
	void Clear();
	bool IsClosed();

	inline GLFWwindow* GetWindow() const { return m_Window; }
	inline const int GetWidth() const { return m_Width; }
	inline const int GetHeight() const { return m_Height; }
private:
	void Init();
private:
	GLFWwindow* m_Window;
	const String& m_Name;
	int m_Width, m_Height;
};