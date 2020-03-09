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

		virtual void OnAttach() {};
		virtual void OnDetach() {};
		
		virtual void OnUpdate() {};
		virtual void OnRender() {};

		virtual Type* GetType() const { return nullptr; }

		inline Ref<SceneObject> GetSceneObject() { return m_sceneObject; }
		inline const Ref<SceneObject> GetSceneObject() const { return m_sceneObject; }

		inline void SetSceneObject(Ref<SceneObject> sceneObject) { m_sceneObject = sceneObject; }

	protected:
		Ref<SceneObject> m_sceneObject;
	};

}