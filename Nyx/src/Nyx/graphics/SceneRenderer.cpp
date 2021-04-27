#include "NXpch.h"
#include "SceneRenderer.h"
#include "Nyx/Graphics/Renderer.h"
#include "Nyx/Asset/AssetManager.h"
#include "Nyx/graphics/RenderPass.h"
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

namespace Nyx
{
    struct SceneRendererData
    {
        Scene* ActiveScene = nullptr;
        Ref<Camera> ActiveCamera;

        std::vector<RenderCommand> RenderCommands;

        Ref<RenderPass> GeometryPass;
        Ref<RenderPass> CompositePass;

        Ref<Shader> SkyboxShader;
        Ref<Shader> PBRShader;
        Ref<Shader> GlassShader;
        Ref<Shader> CompositeShader;
        Ref<Shader> CopyShader;

        Ref<Material> EnvironmentMaterial;

		Ref<EnvironmentMap> SceneEnvironmentMap;
		Ref<LightEnvironment> SceneLightEnvironment;
    };

    static struct SceneRendererData s_Data;

    void SceneRenderer::Init()
    {
        s_Data.SkyboxShader = CreateRef<Shader>("assets/shaders/Skybox.shader");
        s_Data.PBRShader = CreateRef<Shader>("assets/shaders/DefaultPBR.shader");
        s_Data.GlassShader = CreateRef<Shader>("assets/shaders/Glass.shader");
        s_Data.CompositeShader = CreateRef<Shader>("assets/shaders/HDR.shader");
        s_Data.CopyShader = CreateRef<Shader>("assets/shaders/CopyShader.shader");

        s_Data.EnvironmentMaterial = CreateRef<Material>(s_Data.SkyboxShader);
        s_Data.EnvironmentMaterial->SetMaterialFlag(MaterialFlags::DISABLE_DEPTH_TEST);

        {
			FramebufferSpecification fbSpec;
			fbSpec.Scale = 1.0f;
            fbSpec.ColorAttachments = { TextureFormat::RGBA16F };
            fbSpec.DepthAttachments = { TextureFormat::RGBA16F }; 
			RenderPassSpecification renderPassSpec;
            renderPassSpec.Framebuffer = CreateRef<FrameBuffer>(fbSpec);
            s_Data.GeometryPass = CreateRef<RenderPass>(renderPassSpec);
        }

		{
			FramebufferSpecification fbSpec;
			fbSpec.Scale = 1.0f;
			fbSpec.ColorAttachments = { TextureFormat::RGBA };
			fbSpec.DepthAttachments = { TextureFormat::RGBA };
			RenderPassSpecification renderPassSpec;
			renderPassSpec.Framebuffer = CreateRef<FrameBuffer>(fbSpec);
			s_Data.CompositePass = CreateRef<RenderPass>(renderPassSpec);
		}
    }

    void SceneRenderer::Begin(Scene* scene, Ref<Camera> camera)
    {
        s_Data.ActiveScene = scene;
        s_Data.ActiveCamera = camera;
    }

    void SceneRenderer::End()
    {
        // Render
        GeometryPass();
        CompositePass();

        s_Data.RenderCommands.clear();
        s_Data.ActiveScene = nullptr;
    }

    void Nyx::SceneRenderer::SubmitMesh(Ref<Mesh> mesh, glm::mat4 transform)
    {
        // TODO: culling
        s_Data.RenderCommands.push_back(RenderCommand(mesh, transform));
    }

    void SceneRenderer::GeometryPass()
    {   
        Renderer::BeginRenderPass(s_Data.GeometryPass);
		Renderer::BeginScene(s_Data.ActiveCamera);
		Renderer::SetEnvironment(s_Data.SceneEnvironmentMap, s_Data.SceneLightEnvironment);

      Renderer::DrawFullscreenQuad(s_Data.EnvironmentMaterial);

        for (RenderCommand command : s_Data.RenderCommands)
        {
            Renderer::SubmitMesh(command.Mesh, command.Transform, command.MaterialOverride);
        }

        Renderer::EndScene();
        Renderer::EndRenderPass();
    }

    void SceneRenderer::CompositePass()
    {
    //    Renderer::BeginRenderPass(s_Data.CompositePass);
    //    Renderer::DrawFullscreenQuad(s_Data.EnvironmentMaterial);
    //    Renderer::EndRenderPass();
    }

    void Nyx::SceneRenderer::Resize(uint32_t width, uint32_t height)
    {
        s_Data.GeometryPass->GetSpecification().Framebuffer->Resize(width, height);
        s_Data.CompositePass->GetSpecification().Framebuffer->Resize(width, height);
    }

    void SceneRenderer::Blit(Ref<FrameBuffer>& src, Ref<FrameBuffer>& destination, Ref<Shader>& shader, bool clear = true)
    {
        destination->Bind();
        if (clear)
            destination->Clear();

        destination->Unbind();
    }

    Ref<FrameBuffer> Nyx::SceneRenderer::GetFinalBuffer()
    {
        return s_Data.GeometryPass->GetSpecification().Framebuffer;
    }

    Ref<Shader> Nyx::SceneRenderer::GetPBRShader()
    {
        return s_Data.PBRShader;
    }

    Ref<Shader> SceneRenderer::GetGlassShader()
    {
        return s_Data.GlassShader;
    }

	void SceneRenderer::SetEnvironment(Ref<EnvironmentMap> environmentMap, Ref<LightEnvironment> lightEnvironment)
	{
        NX_CORE_ASSERT(s_Data.ActiveScene, "");
        s_Data.SceneEnvironmentMap = environmentMap;
        s_Data.SceneLightEnvironment = lightEnvironment;
	}

}