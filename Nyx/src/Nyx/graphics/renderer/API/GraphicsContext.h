#pragma once
#include "imgui/imgui.h"

struct GLFWwindow;

namespace Nyx {

	class GraphicsContext
	{
	public:
		GraphicsContext(GLFWwindow* window);

	public:
		void Init();
		void SwapBuffers();
		void Clear();

		void RenderImGuiWindow();

	private:
		void UpdateFPSCounter();

		GLFWwindow* m_Window;

		std::vector<float> m_Frames;
		float m_LastTime = 0.0f, m_MSFrame = 0.0f, m_FPS = 0.0f;
		int m_FrameCount = 0;
	};
}