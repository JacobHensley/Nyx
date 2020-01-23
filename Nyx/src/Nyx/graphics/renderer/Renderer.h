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
		static void Begin(Scene* scene);
		static void Flush();
		static void End();

		static void SubmitMesh(Mesh* mesh, glm::mat4 transform, Material* material);

	private:
		static Renderer* s_Instance;
		static Scene* m_ActiveScene;
	};
}