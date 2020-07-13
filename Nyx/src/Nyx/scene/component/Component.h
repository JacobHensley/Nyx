#pragma once
#include "Nyx/UUID.h"

namespace Nyx {

	class SceneObject;

	class Component
	{
	public:
		struct Type
		{
			String Name;
		};

		virtual void OnAttach() {};
		virtual void OnDetach() {};
		
		virtual void OnUpdate() {};
		virtual void OnRender() {};

		virtual Type* GetType() const { return nullptr; }

		virtual UUID GetUUID() { return NULL; }

		inline SceneObject* GetSceneObject() { return m_sceneObject; }
		inline const SceneObject* GetSceneObject() const { return m_sceneObject; }

		inline void SetSceneObject(SceneObject* sceneObject) { m_sceneObject = sceneObject; }

	protected:
		SceneObject* m_sceneObject;
	};

}