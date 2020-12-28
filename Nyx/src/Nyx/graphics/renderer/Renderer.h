#pragma once
#include "nyx/scene/Scene.h"
#include "nyx/graphics/renderer/Mesh.h"
#include "Nyx/graphics/renderer/Material.h"

namespace Nyx {

	class Renderer
	{
	public:
		static void Begin(Ref<Camera> camera);

		static void SubmitMesh(Scene* scene, Ref<Mesh> mesh, glm::mat4 transform);
		static void SubmitMesh(Scene* scene, Ref<Mesh> mesh, glm::mat4 transform, Ref<Material> material);

		static void End();

		static void Init();

	private:
		static void InitRenderFunctions();
		static void InitRenderResourceFunFunctions();
	};
}