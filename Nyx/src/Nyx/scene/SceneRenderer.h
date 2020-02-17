#pragma once
#include "nyx/scene/Scene.h"
#include "nyx/graphics/renderer/Mesh.h"
#include "Nyx/graphics/renderer/Material.h"
#include "Nyx/graphics/renderer/RenderPass.h"

namespace Nyx {

	struct RenderCommand
	{
		RenderCommand(Mesh* mesh, glm::mat4 transform, Material* material)
			: mesh(mesh), transform(transform), material(material)
		{
		}

		Mesh* mesh;
		glm::mat4 transform;
		Material* material;
	};

	class SceneRenderer
	{
	public:
		SceneRenderer();
		~SceneRenderer();

	public:
		static inline void Begin(Scene* scene) { s_Instance->BeginI(scene); }
		static inline void Flush() { s_Instance->FlushI(); }
		static inline void End() { s_Instance->EndI(); }

		static inline void SubmitMesh(Mesh* mesh, glm::mat4 transform, Material* material = nullptr) { s_Instance->SubmitMeshI(mesh, transform, material); }

		static inline void InitGL() { s_Instance->InitGLI(); }

		static inline FrameBuffer* GetFinalBuffer() { return s_Instance->m_FinalBuffer; }
		static inline void SetExposure(float exposure) { s_Instance->m_Exposure = exposure; }
		
	private:
		void BeginI(Scene* scene);
		void FlushI();
		void EndI();

		void SubmitMeshI(Mesh* mesh, glm::mat4 transform, Material* material);

		void InitGLI();

		Scene* m_ActiveScene = nullptr;
		Material* m_ActiveSceneMaterial = nullptr;
		std::vector<RenderCommand> m_RenderCommands;

		FrameBuffer* m_FinalBuffer;

		RenderPass* m_HDRPass;
		FrameBuffer* m_HDRFrameBuffer;
		Shader* m_HDRShader;
		float m_Exposure = 1.0f;

		std::unordered_map<RenderUniformID, std::function<void(const RendererUniform&, RenderCommand&, Shader*)>> m_RendererUniformFuncs;

		static SceneRenderer* s_Instance;
	};
}