#pragma once
#include "SceneObject.h"
#include "Nyx/graphics/renderer/camera/Camera.h"
#include "Nyx/graphics/renderer/API/TextureCube.h"
#include "Nyx/graphics/LightEnvironment.h"

namespace Nyx {

	class Mesh;
	class Material;

	class Scene
	{
	public:
		Scene(Ref<Camera> camera, Ref<LightEnvironment> lightEnv, Ref<TextureCube> skybox);

	public:
		void Update();
		void Render();

		Ref<SceneObject> CreateObject(const String& debugName);
		Ref<SceneObject> CreateObject(std::initializer_list<Ref<Component>> components);
		void AddObject(Ref<SceneObject> object);

		void Remove(const Ref<SceneObject>& sceneObject);

		inline std::vector<Ref<SceneObject>>& GetSceneObjects() { return m_SceneObjects; }
		const Ref<Camera> GetCamera() { return m_Camera; }
		Ref<LightEnvironment> GetLightEnvironment() { return m_LightEnvironment; }

	private:
		std::vector<Ref<SceneObject>> m_SceneObjects;

		Ref<Camera> m_Camera;
		Ref<Mesh> m_SkyboxMesh;
		Ref<Material> m_SkyboxMaterial;
		Ref<LightEnvironment> m_LightEnvironment;

	};

/*	public:
		Scene(Ref<Camera> camera, Ref<LightEnvironment> lightEnv, Ref<TextureCube> skybox);
		~Scene();
	
	public:
		void Update();
		void Render();

		Ref<SceneObject> CreateObject(const String& debugName);
		Ref<SceneObject> CreateObject(std::initializer_list<Ref<Component>> components);

		void AddObject(Ref<SceneObject> object);

		bool RemoveObject(uint index);
		void Remove(const Ref<SceneObject> sceneObject);

		inline std::vector<Ref<SceneObject>>& GetSceneObjects() { return m_SceneObjects; }

		const Ref<Camera> GetCamera() { return m_Camera; }
		Ref<LightEnvironment> GetLightEnvironment() { return m_LightEnvironment; }

	private:
		std::vector<Ref<SceneObject>> m_SceneObjects;

		Ref<Mesh> m_SkyboxMesh;
		Ref<Material> m_SkyboxMaterial;

		Ref<Camera> m_Camera;
		Ref<LightEnvironment> m_LightEnvironment; */
}