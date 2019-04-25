#include <Nyx.h>
#include "layers/GameLayer.h"

int main()
{
	Nyx::Application app = Nyx::Application();
	GameLayer* game = new GameLayer("GameLayer");

	app.PushLayer(game);
	app.Run();
}