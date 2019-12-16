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
		float m_LastTime, m_MSFrame, m_FPS;
		int m_FrameCount;
	};
}