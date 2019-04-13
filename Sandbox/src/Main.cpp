#include <Nyx.h>
#include "layers/GameLayer.h"

int main()
{
	Application app = Application();
	GameLayer* game = new GameLayer("GameLayer");
	app.PushLayer(game);
	app.Run();
}