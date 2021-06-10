#include "NXpch.h"
#include "SceneRenderer.h"
#include "Nyx/Graphics/Renderer.h"
#include "Nyx/Asset/AssetManager.h"
#include "Nyx/graphics/RenderPass.h"
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <imgui/imgui.h>

namespace Nyx
{
    struct SceneRendererData
    {
        Scene* ActiveScene = nullptr;
        Ref<Camera> ActiveCamera;

        std::vector<RenderCommand> RenderCommands;

        Ref<RenderPass> GeometryPass;
        Ref<RenderPass> CompositePass;
        Ref<RenderPass> ShadowPass;

        Ref<Shader> PBRShader;
        Ref<Shader> GlassShader;
        Ref<Shader> CompositeShader;
        Ref<Shader> SkyboxShader;
        Ref<Shader> DepthShader;

		Ref<EnvironmentMap> EnvironmentMap;
		Ref<LightEnvironment> LightEnvironment;

        Ref<Camera> ShadowCamera;
        Ref<Material> DepthMaterial;

        bool ClearShadowBuffer;
        struct ShadowSettings
        {
            float Size = 16.0f;
            float NearClip = -15.0f;
            float FarClip = 15.0f;

            float MaxShadowDistance = 200.0f;
            float ShadowFade = 1.0f;
        } ShadowSetting;
    };

    static struct SceneRendererData s_Data;

    void SceneRenderer::Init()
    {
        s_Data.PBRShader = CreateRef<Shader>("assets/shaders/PBR.shader");
        s_Data.GlassShader = CreateRef<Shader>("assets/shaders/Glass.shader");
        s_Data.CompositeShader = CreateRef<Shader>("assets/shaders/Composite.shader");
        s_Data.SkyboxShader = CreateRef<Shader>("assets/shaders/Skybox.shader");
        s_Data.DepthShader = CreateRef<Shader>("assets/shaders/Depth.shader");

        // Geometry Pass
        {
			FramebufferSpecification fbSpec;
			fbSpec.Scale = 1.0f;
            fbSpec.ColorAttachments = { TextureFormat::RGBA16F };
            fbSpec.DepthAttachments = { TextureFormat::RGBA16F }; 
			RenderPassSpecification renderPassSpec;
            renderPassSpec.Framebuffer = CreateRef<FrameBuffer>(fbSpec);
            s_Data.GeometryPass = CreateRef<RenderPass>(renderPassSpec);
        }

        // Composite Pass
		{
			FramebufferSpecification fbSpec;
			fbSpec.Scale = 1.0f;
			fbSpec.ColorAttachments = { TextureFormat::RGBA };
			fbSpec.DepthAttachments = { TextureFormat::RGBA };
			RenderPassSpecification renderPassSpec;
			renderPassSpec.Framebuffer = CreateRef<FrameBuffer>(fbSpec);
			s_Data.CompositePass = CreateRef<RenderPass>(renderPassSpec);
		}

        // Shadow Pass
        {
            FramebufferSpecification fbSpec;
            fbSpec.Scale = 1.0f;
            fbSpec.Width = 4096;
            fbSpec.Height = 4096;
            fbSpec.DepthAttachments = { TextureFormat::DEPTH32F };
            RenderPassSpecification renderPassSpec;
            renderPassSpec.Framebuffer = CreateRef<FrameBuffer>(fbSpec);
            s_Data.ShadowPass = CreateRef<RenderPass>(renderPassSpec);
        }

        s_Data.ClearShadowBuffer = false;
        s_Data.DepthMaterial = CreateRef<Material>(s_Data.DepthShader);
        s_Data.ShadowCamera = CreateRef<Camera>(glm::ortho(-16.0f, 16.0f, -16.0f, 16.0f, -15.0f, 15.0f));
    }

    void SceneRenderer::Begin(Scene* scene, Ref<Camera> camera)
    {
        s_Data.ActiveScene = scene;
        s_Data.ActiveCamera = camera;
    }

    void SceneRenderer::End()
    {
        Renderer::SetEnvironment(s_Data.EnvironmentMap, s_Data.LightEnvironment);

        // Render
        ShadowPass();
        GeometryPass();
        CompositePass();

        s_Data.LightEnvironment->Clear();

        s_Data.RenderCommands.clear();
        s_Data.ActiveScene = nullptr;
    }

    void Nyx::SceneRenderer::SubmitMesh(Ref<Mesh> mesh, glm::mat4 transform)
    {
        // TODO: culling
        s_Data.RenderCommands.push_back(RenderCommand(mesh, transform));
    }

    void SceneRenderer::ShadowPass()
    {
        if (s_Data.LightEnvironment->GetDirectionalLights().size() > 0 && s_Data.LightEnvironment->GetDirectionalLights()[0]->Active)
        {
            s_Data.ShadowCamera->SetProjectionMatrix(glm::ortho(-s_Data.ShadowSetting.Size, s_Data.ShadowSetting.Size, -s_Data.ShadowSetting.Size, s_Data.ShadowSetting.Size, s_Data.ShadowSetting.NearClip, s_Data.ShadowSetting.FarClip));

            glm::vec3 lightDirection = s_Data.LightEnvironment->GetDirectionalLights()[0]->Direction;
            s_Data.ShadowCamera->SetViewMatrix(glm::lookAt(lightDirection, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));

            Renderer::BeginRenderPass(s_Data.ShadowPass);
            Renderer::BeginScene(s_Data.ShadowCamera);

            for (RenderCommand command : s_Data.RenderCommands)
            {
                Renderer::SubmitMesh(command.Mesh, command.Transform, s_Data.DepthMaterial);
            }

            Renderer::EndScene();
            Renderer::EndRenderPass();

            s_Data.ClearShadowBuffer = true;
        }
        else if (s_Data.ClearShadowBuffer)
        {
            s_Data.ShadowPass->GetSpecification().Framebuffer->Bind();
            s_Data.ShadowPass->GetSpecification().Framebuffer->Clear();
            s_Data.ClearShadowBuffer = false;
        }

        // Should we upload the shadow map buffer every frame even if there is no active light?
        Renderer::SetShadowMap(s_Data.ShadowCamera->GetProjectionMatrix() * s_Data.ShadowCamera->GetViewMatrix(), s_Data.ShadowPass->GetSpecification().Framebuffer->GetDepthAttachment());
    }

    void SceneRenderer::GeometryPass()
    {
        glProgramUniform1f(s_Data.PBRShader->GetShaderProgram(), 10, s_Data.ShadowSetting.MaxShadowDistance);
        glProgramUniform1f(s_Data.PBRShader->GetShaderProgram(), 11, s_Data.ShadowSetting.ShadowFade);

        Renderer::BeginRenderPass(s_Data.GeometryPass);
        Renderer::BeginScene(s_Data.ActiveCamera);

        Renderer::DrawFullscreenQuad(s_Data.SkyboxShader, false);

        for (RenderCommand command : s_Data.RenderCommands)
        {
            Renderer::SubmitMesh(command.Mesh, command.Transform, command.MaterialOverride);
        }

        Renderer::EndScene();
        Renderer::EndRenderPass();
    }

    void SceneRenderer::CompositePass()
    {
        Renderer::BeginRenderPass(s_Data.CompositePass);

        s_Data.CompositeShader->Bind();
        s_Data.CompositeShader->SetUniformFloat("u_Material.Exposure", s_Data.ActiveCamera->GetExposure());
        s_Data.GeometryPass->GetSpecification().Framebuffer->BindColorTexture(0, 0);

        Renderer::DrawFullscreenQuad(s_Data.CompositeShader, false);

        Renderer::EndRenderPass();
    }

    void SceneRenderer::SetEnvironment(Ref<EnvironmentMap> environmentMap, Ref<LightEnvironment> lightEnvironment)
    {
        s_Data.EnvironmentMap = environmentMap;
        s_Data.LightEnvironment = lightEnvironment;
    }
    
    void Nyx::SceneRenderer::Resize(uint32_t width, uint32_t height)
    {
        s_Data.GeometryPass->GetSpecification().Framebuffer->Resize(width, height);
        s_Data.CompositePass->GetSpecification().Framebuffer->Resize(width, height);
    }

    void SceneRenderer::OnImGuiRender()
    {
        ImGui::Begin("Scene Renderer");

        ImGui::DragFloat("Size", &s_Data.ShadowSetting.Size);
        ImGui::DragFloat("Near Clip", &s_Data.ShadowSetting.NearClip);
        ImGui::DragFloat("Far Clip", &s_Data.ShadowSetting.FarClip);
        ImGui::DragFloat("Max Shadow Distance", &s_Data.ShadowSetting.MaxShadowDistance);
        ImGui::DragFloat("Shadow Fade", &s_Data.ShadowSetting.ShadowFade);

        auto size = ImGui::GetContentRegionAvail();
        ImGui::Image((ImTextureID)s_Data.ShadowPass->GetSpecification().Framebuffer->GetDepthAttachment(), { size.x, size.x }, { 0, 1 }, { 1, 0 });

        ImGui::End();
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
        return s_Data.CompositePass->GetSpecification().Framebuffer;
    }

    Ref<Shader> Nyx::SceneRenderer::GetPBRShader()
    {
        return s_Data.PBRShader;
    }

    Ref<Shader> SceneRenderer::GetGlassShader()
    {
        return s_Data.GlassShader;
    }

    Ref<Shader> SceneRenderer::GetCompositeShader()
    {
        return s_Data.CompositeShader;
    }

    Ref<Shader> SceneRenderer::GetSkyboxShader()
    {
        return s_Data.SkyboxShader;
    }

}