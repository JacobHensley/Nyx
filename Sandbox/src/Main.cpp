#include <Nyx.h>
#include "layers/GameLayer.h"
#include "Layers/ViewerLayer.h"

int main()
{
	Application app = Application();
//	GameLayer* game = new GameLayer("GameLayer");
	
	ViewerLayer* viewer = new ViewerLayer("ViewerLayer");

	app.PushLayer(viewer);
//	app.PushLayer(game);
	app.Run();
}