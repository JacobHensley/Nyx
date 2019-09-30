#pragma once
#include "glm/glm.hpp"

namespace Nyx {

	class Input
	{
	public:
		static bool IsKeyPressed(int keycode);
		static bool IsMouseButtonPressed(int button);
		static glm::vec2 GetMousePos();

		static float GetMouseX();
		static float GetMouseY();
	private:
		static Input* s_Instance;
	};
}