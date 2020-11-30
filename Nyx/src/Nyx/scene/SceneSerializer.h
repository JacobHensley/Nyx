#pragma once
#include "Nyx/scene/Scene.h"

namespace Nyx
{
	class SceneSerializer
	{
	public:
		static void Save(Ref<Scene>& scene, const String& path);
		static Ref<Scene> Load(const String& path);
	};
}