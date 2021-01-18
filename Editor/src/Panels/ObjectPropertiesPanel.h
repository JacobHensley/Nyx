#pragma once
#include "Nyx/Core/Core.h"
#include "Nyx/Scene/SceneObject.h"

class ObjectPropertiesPanel
{
public:
	ObjectPropertiesPanel();

	void Draw(Nyx::Ref<Nyx::SceneObject> object);

private:
	void DrawNameInput(Nyx::Ref<Nyx::SceneObject> object);
	void DrawAddComponents(Nyx::Ref<Nyx::SceneObject> object);
};