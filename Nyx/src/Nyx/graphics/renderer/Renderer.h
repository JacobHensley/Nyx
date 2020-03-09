#pragma once
#include "nyx/scene/Scene.h"
#include "nyx/graphics/renderer/Mesh.h"
#include "Nyx/graphics/renderer/Material.h"

namespace Nyx {

	class Renderer
	{
	public:
		Renderer();
		~Renderer();

	public:
		static void Begin();
		static void Flush();
		static void End();

		static void SubmitMesh(Ref<Mesh> mesh, glm::mat4 transform, Ref<Material> material, Scene* scene);

	private:
		static Renderer* s_Instance;
	};
}