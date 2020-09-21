#pragma once
#include "nyx/scene/Scene.h"
#include "Nyx/graphics/renderer/Shader.h"
#include "nyx/graphics/renderer/Mesh.h"

namespace Nyx {

	class RenderPass
	{
	public:
		RenderPass()
		{
			m_FrameBuffer = CreateRef<FrameBuffer>(1280, 720);
			m_FrameBuffer->Bind();
			m_FrameBuffer->Attach(BufferAtachment::COLOR);
			m_FrameBuffer->Attach(BufferAtachment::DEPTH);
			m_FrameBuffer->Unbind();
		}

		RenderPass(TextureParameters& textureParameters)
		{
			m_FrameBuffer = CreateRef<FrameBuffer>(1280, 720, textureParameters);
			m_FrameBuffer->Bind();
			m_FrameBuffer->Attach(BufferAtachment::COLOR);
			m_FrameBuffer->Attach(BufferAtachment::DEPTH);
			m_FrameBuffer->Unbind();
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

	enum class MaterialFilter
	{
		NONE = -1, NORMALS, TEXTURE_COORDS
	};

	class SceneRenderer
	{
	public:
		static void Init();

		static void Begin(Scene* scene);
		static void Flush();
		static void End();

		static void SubmitMesh(Ref<Mesh> mesh, glm::mat4 transform);
		static void SubmitMesh(Ref<Mesh> mesh, glm::mat4 transform, Ref<Material> materialOverride);

		static Ref<FrameBuffer> GetFinalBuffer();

		// Temp functions
		static void Resize(uint width, uint height);
		static Ref<Shader> GetPBRShader();

	private:
		static void GeometryPass();
		static void CompositePass();
		static void JumpFloodPass();

		static void Blit(Ref<FrameBuffer>& src, Ref<FrameBuffer>& dest, Ref<Shader>& shader);
	};
}