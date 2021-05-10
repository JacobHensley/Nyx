#include "EditorLayer.h"

using namespace Nyx;

int main()
{
	Ref<Application> application = CreateRef<Application>();
	Ref<EditorLayer> editor = CreateRef<EditorLayer>();

	application->PushLayer(editor);
	application->Run();

	return 0;
}