#pragma once
#include "Nyx/Core/Core.h"
#include "Nyx/Scene/Scene.h"
#include "Nyx/Graphics/API/FrameBuffer.h"
#include "Nyx/Graphics/Shader.h"
#include "Nyx/Graphics/Mesh.h"

namespace Nyx {

	class RenderPass
	{
	public:
		RenderPass()
		{
			TextureParameters params;
			params.format = TextureFormat::RGBA16F;

			FramebufferSpecification fbSpec;
			fbSpec.Scale = 1.0f;
			fbSpec.ColorAttachments.emplace_back(params);
			fbSpec.DepthAttachments.emplace_back(params);
			m_FrameBuffer = CreateRef<FrameBuffer>(fbSpec);
		}

		RenderPass(TextureParameters& textureParameters)
		{
			FramebufferSpecification fbSpec;
			fbSpec.Scale = 1.0f;
			fbSpec.ColorAttachments.emplace_back(textureParameters);
			fbSpec.DepthAttachments.emplace_back(textureParameters);
			m_FrameBuffer = CreateRef<FrameBuffer>(fbSpec);
		}

		void Bind()
		{
			m_FrameBuffer->Bind();
			m_FrameBuffer->Clear();
		}

		void Unbind()
		{
			m_FrameBuffer->Unbind();
		}

		Ref<FrameBuffer> GetFrameBuffer() { return m_FrameBuffer; }

	private:
		Ref<FrameBuffer> m_FrameBuffer;
	};

	struct RenderCommand
	{
		RenderCommand(Ref<Mesh> mesh, glm::mat4 transform, Ref<Material> materialOverride)
			: mesh(mesh), transform(transform), materialOverride(materialOverride)
		{
		}
		
		RenderCommand(Ref<Mesh> mesh, glm::mat4 transform)
			: mesh(mesh), transform(transform), materialOverride(nullptr)
		{
		}

		Ref<Mesh> mesh;
		glm::mat4 transform;
		Ref<Material> materialOverride;
	};


	class SceneRenderer
	{
	public:
		static void Init();

		static void Begin(Scene* scene, Ref<Camera> camera);
		static void Flush();
		static void End();

		static void SubmitMesh(Ref<Mesh> mesh, glm::mat4 transform);
		static void SubmitMesh(Ref<Mesh> mesh, glm::mat4 transform, Ref<Material> materialOverride);
		static void SubmitSelectedMesh(Ref<Mesh> mesh, glm::mat4 transform, Ref<Material> materialOverride);

		static Ref<FrameBuffer> GetFinalBuffer();

		// Temp functions
		static void Resize(uint width, uint height);
		static Ref<Shader> GetPBRShader();

	private:
		static void GeometryPass();
		static void CompositePass();
		static void JumpFloodPass();

		static void Blit(Ref<FrameBuffer>& src, Ref<FrameBuffer>& dest, Ref<Shader>& shader, bool clear);
	};
}