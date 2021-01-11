#pragma once
#include "Nyx/Core/Core.h"
#include "Nyx/Scene/Scene.h"
#include <String>

namespace Nyx
{
	class SceneSerializer
	{
	public:
		static void Save(Ref<Scene>& scene, const std::string& path);
		static Ref<Scene> Load(const std::string& path);
	};
}