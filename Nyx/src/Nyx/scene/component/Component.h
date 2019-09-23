#pragma once

namespace Nyx {

	class SceneObject;

	class Component
	{
	public:
		struct Type
		{
			String Name;
		};
	public:

		virtual Type* GetType() const { return nullptr; }

		inline SceneObject* GetSceneObject() { return m_sceneObject; }
		inline const SceneObject* GetSceneObject() const { return m_sceneObject; }
		inline void SetSceneObject(SceneObject* sceneObject) { m_sceneObject = sceneObject; }

	protected:
		SceneObject* m_sceneObject;
	};

}