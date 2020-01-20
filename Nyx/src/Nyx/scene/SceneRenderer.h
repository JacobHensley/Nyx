#pragma once
#include "nyx/scene/Scene.h"
#include "nyx/graphics/renderer/Mesh.h"
#include "Nyx/graphics/renderer/Material.h"

namespace Nyx {

	class SceneRenderer
	{
	public:
		SceneRenderer();
		~SceneRenderer();

	public:
		static void Begin(Scene* scene, Material* sceneMaterialOverride = nullptr);
		static void Flush();
		static void End();

		static void SubmitMesh(Mesh* mesh, glm::mat4 transform, bool depthTesting = true, Material* objectMaterialOverride = nullptr);

	private:
		static Scene* m_ActiveScene;
		static Material* m_ActiveSceneMaterial;

		static SceneRenderer* s_Instance;
	};
}