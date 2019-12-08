#pragma once

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

	private:
		GLFWwindow* m_Window;
	};
}