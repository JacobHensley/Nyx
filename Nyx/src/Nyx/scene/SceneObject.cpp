#include "NXpch.h"
#include "SceneObject.h"
#include "Components.h"

namespace Nyx 
{
	const String& SceneObject::GetObjectName()
	{
		if (HasComponent<TagComponent>())
		{
			return GetComponent<TagComponent>().Tag;
		}
		else
		{
			return "Untitled Object";
		}
	}
}