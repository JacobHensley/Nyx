#pragma once
#include "nyx/scene/Scene.h"
#include "nyx/graphics/renderer/Mesh.h"
#include "Nyx/graphics/renderer/Material.h"

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
		RenderCommand(Ref<Mesh> mesh, glm::mat4 transform, Ref<Material> material)
			: mesh(mesh), transform(transform), material(material)
		{
		}

		Ref<Mesh> mesh;
		glm::mat4 transform;
		Ref<Material> material;
	};

	enum class MaterialFilter
	{
		NONE = -1, NORMALS, TEXTURE_COORDS, WORLD_NORMALS
	};

	class SceneRenderer
	{
	public:
		static void Init();

		static void Begin(Scene* scene);
		static void Flush();
		static void End();

		static void SubmitMesh(Ref<Mesh> mesh, glm::mat4 transform, Ref<Material> material = nullptr);

		static void SetMaterialFilter(MaterialFilter filter);

		static void Resize(uint width, uint height); //Remove later?

		static Ref<Shader> GetPBRShader();
		static Ref<FrameBuffer> GetFinalBuffer();

	private:
		static void InitRenderFunctions();
		static void InitRenderResourceFunFunctions();
	};
}