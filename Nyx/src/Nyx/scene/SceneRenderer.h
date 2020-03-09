#pragma once
#include "nyx/scene/Scene.h"
#include "nyx/graphics/renderer/Mesh.h"
#include "Nyx/graphics/renderer/Material.h"
#include "Nyx/graphics/renderer/RenderPass.h"

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
		SceneRenderer();
		~SceneRenderer();

	public:
		static inline void Begin(Scene* scene) { s_Instance->BeginI(scene); }
		static inline void Flush() { s_Instance->FlushI(); }
		static inline void End() { s_Instance->EndI(); }

		static inline void SubmitMesh(Ref<Mesh> mesh, glm::mat4 transform, Ref<Material> material = nullptr) { s_Instance->SubmitMeshI(mesh, transform, material); }

		static inline void InitGL() { s_Instance->InitGLI(); }

		static inline Ref<Shader> GetPBRShader() { return s_Instance->GetPBRShaderI(); }

		static inline Ref<FrameBuffer> GetFinalBuffer() { return s_Instance->m_CompositeBuffer; }
		static inline void SetExposure(float exposure) { s_Instance->m_Exposure = exposure; }

		static inline void Resize(uint32_t width, uint32_t height) { s_Instance->ResizeI(width, height); }
	private:
		void BeginI(Scene* scene);
		void FlushI();
		void EndI();

		void ResizeI(uint32_t width, uint32_t height);
		Ref<Shader> GetPBRShaderI() { return m_PBRShader; }
		void SubmitMeshI(Ref<Mesh> mesh, glm::mat4 transform, Ref<Material> material);

		void InitGLI();

	private:
		Scene* m_ActiveScene = nullptr;
		Ref<Material> m_ActiveSceneMaterial = nullptr;

		Ref<Shader> m_PBRShader;

		std::vector<RenderCommand> m_RenderCommands;

		Ref<FrameBuffer> m_CompositeBuffer = nullptr;
		Ref<FrameBuffer> m_GeometryBuffer = nullptr;

		Ref<Shader> m_CompositeShader = nullptr;
		Ref<Mesh> m_FullscreenQuad = nullptr;
		float m_Exposure = 1.0f;

		std::unordered_map<RenderUniformID, std::function<void(const RendererUniform&, RenderCommand&, Ref<Shader>)>> m_RendererUniformFuncs;
		static SceneRenderer* s_Instance;
	};
}