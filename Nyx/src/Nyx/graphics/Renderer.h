#pragma once
#include "Nyx/Core/Core.h"
#include "Nyx/Scene/Scene.h"
#include "Nyx/Graphics/Mesh.h"
#include "Nyx/Graphics/Material.h"

namespace Nyx {

	class Renderer
	{
	public:
		static void Begin(Ref<Camera> camera, Scene* scene);

		static void SubmitMesh(Scene* scene, Ref<Mesh> mesh, const glm::mat4& transform);
		static void SubmitMesh(Scene* scene, Ref<Mesh> mesh, const glm::mat4& transform, Ref<Material> material);
		static void SubmitFullscreenQuad(Ref<Material> material);

		static void End();

		static void Flush();

		static void Init();

	private:
		static void InitRenderFunctions();
		static void InitRenderResourceFunFunctions();
	};
}