#pragma once

namespace Nyx {

	class Input
	{
	public:
		bool IsKeyPressed(int keycode);
		bool IsMouseButtonPressed(int keycode);
		std::pair<float, float> GetMousePos();

		float GetMouseX();
		float GetMouseY();
	};
}