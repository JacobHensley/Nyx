#include <Nyx.h>
#include "Layers/ViewerLayer.h"
#include "Layers/EditorLayer.h"

#define UserType(name, ...) sol::usertype<name> type = Application::GetLuaState().new_usertype<name>(#name, sol::constructors<__VA_ARGS__>());
#define Bind(name, ...) type[name] = __VA_ARGS__;

int main()
{
	Application app = Application();

//	ViewerLayer* viewer = new ViewerLayer("ViewerLayer");
//	app.PushLayer(viewer);

	Ref<EditorLayer> editor = CreateRef<EditorLayer>("Editor");
	app.PushLayer(editor);

	app.Run();
}