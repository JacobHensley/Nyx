#include <Nyx.h>
#include "Layers/ViewerLayer.h"

int main()
{
	Application app = Application();

	ViewerLayer* viewer = new ViewerLayer("ViewerLayer");
	app.PushLayer(viewer);

	app.Run();
}