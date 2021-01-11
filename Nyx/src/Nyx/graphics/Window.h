#pragma once
#include "Nyx/Core/Core.h"
#include "Nyx/Input/Input.h"
#include "Nyx/Events/Event.h"
#include "Nyx/Graphics/GraphicsContext.h"
#include <GLFW/glfw3.h>
#include <String>

namespace Nyx {

	using EvenCallbackFn = std::function<void(Event&)>;

	class Window
	{
	public:
		Window(const std::string& name, int width, int height);
		~Window();

	public:
		void Update();
		void Clear();
		void ImGuiRender();

		bool IsClosed();

		inline GLFWwindow* GetGLFWWindow() const { return m_Window; }

		inline void RenderGraphicsContex(bool value) { m_RenderContext = value; }
		inline const bool GetContextFlag() { return m_RenderContext; }

		inline const int GetWidth() const { return m_Width; }
		inline const int GetHeight() const { return m_Height; }

		inline const glm::vec2 GetWindowPos() const { return m_WindowPos; }

		inline void SetEventCallback(const EvenCallbackFn& callback) { m_EventCallback = callback; };

	private:
		void Init();

	private:
		GLFWwindow* m_Window;
		Ref<GraphicsContext> m_Context;

		bool m_RenderContext = false;
		const std::string m_Name;
		int m_Width, m_Height;

		glm::vec2 m_WindowPos;

		EvenCallbackFn m_EventCallback;
	};

}