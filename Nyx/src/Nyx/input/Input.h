#pragma once
#include "glm/glm.hpp"

namespace Nyx {

	class Input
	{
	public:
		bool IsKeyPressed(int keycode);
		bool IsMouseButtonPressed(int keycode);
		glm::vec2 GetMousePos();

		float GetMouseX();
		float GetMouseY();
	};
}