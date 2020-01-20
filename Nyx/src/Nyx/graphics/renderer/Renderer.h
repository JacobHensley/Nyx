#pragma once
#include "nyx/scene/Scene.h"
#include "nyx/graphics/renderer/Mesh.h"
#include "Nyx/graphics/renderer/Material.h"

namespace Nyx {

	struct RenderCommand
	{
		class RenderCommand(Mesh* mesh, glm::mat4 transform, Material* material, bool depthTesting)
			:	mesh(mesh), transform(transform), material(material), depthTesting(depthTesting)
		{
		}

		Mesh* mesh;
		glm::mat4 transform;
		Material* material;
		bool depthTesting;
	};

	class Renderer
	{
	public:
		Renderer();
		~Renderer();

	public:
		static void Begin(Scene* scene);
		static void Flush();
		static void End();

		static void SubmitMesh(Mesh* mesh, glm::mat4 transform, Material* material, bool depthTesting);

		static FrameBuffer* GetFinalBuffer() { return m_FinalBuffer; }
		static inline void SetExposure(float exposure) { m_Exposure = exposure; }

		static void InitGL();

	private:
		static Renderer* s_Instance;

		static Scene* m_ActiveScene;

		static Shader* m_HDRShader;
		static FrameBuffer* m_HDRBuffer;
		static FrameBuffer* m_FinalBuffer;
		static Mesh* m_FullscreenQuad;

		static std::vector<RenderCommand> m_Commands;

		static float m_Exposure;
	};
}