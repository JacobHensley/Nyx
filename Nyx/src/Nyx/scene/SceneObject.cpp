#include "NXpch.h"
#include "SceneObject.h"
#include "Nyx/Scene/Components.h"
#include "Nyx/Scene/Scene.h"

namespace Nyx 
{
	const std::string& SceneObject::GetObjectName()
	{
		if (HasComponent<TagComponent>())
		{
			return GetComponent<TagComponent>().Tag;
		}
		else
		{
			static std::string untitled = "Untitled Object";
			return untitled;
		}
	}
}