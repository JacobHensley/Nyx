#pragma once
#include "SceneObject.h"
#include "Nyx/graphics/renderer/camera/Camera.h"
#include "Nyx/graphics/renderer/API/TextureCube.h"

namespace Nyx {

	class Mesh;
	class Material;

	class Scene
	{
	public:
		Scene(Camera* camera, TextureCube* skybox);
		~Scene();
	
	public:
		void Update();
		void Render();

		SceneObject* CreateObject();
		SceneObject* CreateObject(std::initializer_list<Component*> components);

		void AddObject(SceneObject& object);

		void Remove(const SceneObject& sceneObject);
		void RemoveAll();

		void AddComponent(Component* component);

		template<typename T>
		T* GetComponent(const SceneObject* sceneObject)
		{
			static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");
			return m_ComponentCache.Get<T>(sceneObject);
		}

		const Camera* GetCamera() { return m_Camera; }

	private:
		ComponentCache m_ComponentCache;
		std::vector<SceneObject> m_SceneObjects;

		Mesh* m_SkyboxMesh;
		Material* m_SkyboxMaterial;

		Camera* m_Camera;
	
	};

}