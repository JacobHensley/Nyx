#pragma once
#include "nyx/scene/Scene.h"
#include "nyx/graphics/renderer/Mesh.h"
#include "Nyx/graphics/renderer/Material.h"

namespace Nyx {

	struct RenderCommand
	{
		RenderCommand(Ref<Mesh> mesh, glm::mat4 transform, Ref<Material> material)
			: mesh(mesh), transform(transform), material(material)
		{
		}

		Ref<Mesh> mesh;
		glm::mat4 transform;
		Ref<Material> material;
	};

	class SceneRenderer
	{
	public:
		static void Init();

		static void Begin(Scene* scene);
		static void Flush();
		static void End();

		static void SubmitMesh(Ref<Mesh> mesh, glm::mat4 transform, Ref<Material> material = nullptr);

		static void Resize(uint width, uint height); //Remove later?

		static Ref<Shader> GetPBRShader();
		static Ref<FrameBuffer> GetFinalBuffer();

	private:
		static void InitRenderFunctions();
		static void InitRenderResourceFunFunctions();
	};
}