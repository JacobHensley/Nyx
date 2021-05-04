#include "Nyx/Core/Application.h"
#include "EditorLayer.h"

int main()
{
	Ref<Application> application = CreateRef<Application>();
	Ref<EditorLayer> editor = CreateRef<EditorLayer>();

	application->PushLayer(editor);
	application->Run();

	return 0;
}