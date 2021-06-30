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
        Ref<RenderPass> MaskPass;
        Ref<RenderPass> CompositePass;
        Ref<RenderPass> ShadowPass;

        Ref<Shader> PBRShader;
        Ref<Shader> GlassShader;
        Ref<Shader> CompositeShader;
        Ref<Shader> SkyboxShader;
        Ref<Shader> DepthShader;
        Ref<Shader> EquirectToCubemapShader;
        Ref<Shader> FilterCubemapShader;
        Ref<Shader> ComputeIrradianceMapShader;
        Ref<Shader> CopyShader;
        Ref<Shader> MaskShader;

        Ref<Shader> JumpFloodInitShader;
        Ref<Shader> JumpFloodShader;
        Ref<Shader> FinalJumpFloodShader;

        Ref<FrameBuffer> JumpFloodBuffer1;
        Ref<FrameBuffer> JumpFloodBuffer2;

        Ref<Material> MaskMaterial;
        Ref<Material> DepthMaterial;

		Ref<EnvironmentMap> EnvironmentMap;
		Ref<LightEnvironment> LightEnvironment;

        uint32_t UseIrradianceSkybox;
        uint32_t SelectedObjectID;

        Ref<Camera> ShadowCamera;

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
        // Shaders - Move to shader libary
        s_Data.PBRShader = CreateRef<Shader>("assets/shaders/PBR.shader");
        s_Data.GlassShader = CreateRef<Shader>("assets/shaders/Glass.shader");
        s_Data.CompositeShader = CreateRef<Shader>("assets/shaders/Composite.shader");
        s_Data.SkyboxShader = CreateRef<Shader>("assets/shaders/Skybox.shader");
        s_Data.DepthShader = CreateRef<Shader>("assets/shaders/Depth.shader");
        s_Data.EquirectToCubemapShader = CreateRef<Shader>("assets/shaders/EquirectToCubemap.shader");
        s_Data.FilterCubemapShader = CreateRef<Shader>("assets/shaders/FilterCubemap.shader");
        s_Data.ComputeIrradianceMapShader = CreateRef<Shader>("assets/shaders/ComputeIrradianceMap.shader");

        s_Data.JumpFloodInitShader = CreateRef<Shader>("assets/shaders/JumpFlood/JumpFloodInit.shader");
        s_Data.JumpFloodShader = CreateRef<Shader>("assets/shaders/JumpFlood/JumpFlood.shader");
        s_Data.FinalJumpFloodShader = CreateRef<Shader>("assets/shaders/JumpFlood/FinalJumpFlood.shader");
        s_Data.CopyShader = CreateRef<Shader>("assets/shaders/Copy.shader");
        s_Data.MaskShader = CreateRef<Shader>("assets/shaders/Mask.shader");

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

        // Mask Pass
        {
            FramebufferSpecification fbSpec;
            fbSpec.Scale = 1.0f;
            fbSpec.ColorAttachments = { TextureFormat::RGBA };
            RenderPassSpecification renderPassSpec;
            renderPassSpec.Framebuffer = CreateRef<FrameBuffer>(fbSpec);
            s_Data.MaskPass = CreateRef<RenderPass>(renderPassSpec);
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
        
        // JumpFlood buffer 1
        {
            FramebufferSpecification fbSpec;
            fbSpec.Scale = 1.0f;
            fbSpec.ColorAttachments = { TextureFormat::RGBA16F };
            s_Data.JumpFloodBuffer1 = CreateRef<FrameBuffer>(fbSpec);
        }

        // JumpFlood buffer 2
        {
            FramebufferSpecification fbSpec;
            fbSpec.Scale = 1.0f;
            fbSpec.ColorAttachments = { TextureFormat::RGBA16F };
            s_Data.JumpFloodBuffer2 = CreateRef<FrameBuffer>(fbSpec);
        }

        s_Data.MaskMaterial = CreateRef<Material>(s_Data.MaskShader);
        s_Data.DepthMaterial = CreateRef<Material>(s_Data.DepthShader);

        s_Data.ClearShadowBuffer = false;
        s_Data.ShadowCamera = CreateRef<Camera>(glm::ortho(-16.0f, 16.0f, -16.0f, 16.0f, -15.0f, 15.0f));

        s_Data.UseIrradianceSkybox = false;
        s_Data.SelectedObjectID = -1;
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
        OutlinePass();
        CompositePass();

        s_Data.LightEnvironment->Clear();

        s_Data.RenderCommands.clear();
        s_Data.ActiveScene = nullptr;
        s_Data.SelectedObjectID = -1;
    }

    void SceneRenderer::SubmitMesh(Ref<Mesh> mesh, glm::mat4 transform)
    {
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

        Renderer::SetShadowMap(s_Data.ShadowCamera->GetProjectionMatrix() * s_Data.ShadowCamera->GetViewMatrix(), s_Data.ShadowPass->GetSpecification().Framebuffer->GetDepthAttachment());
    }

    void SceneRenderer::GeometryPass()
    {
        // Move into scene setting uniform buffer
        glProgramUniform1f(s_Data.PBRShader->GetShaderProgram(), 10, s_Data.ShadowSetting.MaxShadowDistance);
        glProgramUniform1f(s_Data.PBRShader->GetShaderProgram(), 11, s_Data.ShadowSetting.ShadowFade);
        glProgramUniform1i(s_Data.SkyboxShader->GetShaderProgram(), 0, s_Data.UseIrradianceSkybox);

        Renderer::BeginRenderPass(s_Data.GeometryPass);
        Renderer::BeginScene(s_Data.ActiveCamera);

        Renderer::DrawFullscreenQuad(s_Data.SkyboxShader, false);

        for (RenderCommand command : s_Data.RenderCommands)
        {
            Renderer::SubmitMesh(command.Mesh, command.Transform, nullptr);
        }

        Renderer::EndScene();
        Renderer::EndRenderPass();
    }

    static int s_Steps = 3;
    void SceneRenderer::OutlinePass()
    {   
        // Don't do the pass if there is no selected object
        if (s_Data.SelectedObjectID != -1)
        {
            // Render mask pass
            Renderer::BeginRenderPass(s_Data.MaskPass);
            Renderer::BeginScene(s_Data.ActiveCamera);

            int i = 0;
            for (RenderCommand command : s_Data.RenderCommands)
            {
                if (i == s_Data.SelectedObjectID)
                    Renderer::SubmitMesh(command.Mesh, command.Transform, s_Data.MaskMaterial);
                i++;
            }

            Renderer::EndScene();
            Renderer::EndRenderPass();

            // Set state for pass
            glBlendFunc(GL_ONE, GL_ZERO);

            // Initial pass
            s_Data.JumpFloodInitShader->Bind();
            s_Data.JumpFloodInitShader->SetUniformFloat2("u_TextureSize", glm::vec2(1.0f / 1280.0f, 1.0f / 720.0f));
            Blit(s_Data.MaskPass->GetSpecification().Framebuffer, s_Data.JumpFloodBuffer1, s_Data.JumpFloodInitShader, true);

            // Jumpflood cycle
            int step = std::round(std::pow(s_Steps - 1, 2));
            while (step != 0) {

                s_Data.JumpFloodShader->Bind();
                s_Data.JumpFloodShader->SetUniformInt("u_Step", step);
                s_Data.JumpFloodShader->SetUniformFloat2("u_TexelSize", glm::vec2(1.0f / 1280.0f, 1.0f / 720.0f));
                Blit(s_Data.JumpFloodBuffer1, s_Data.JumpFloodBuffer2, s_Data.JumpFloodShader, true);

                Ref<FrameBuffer> tmp = s_Data.JumpFloodBuffer1;
                s_Data.JumpFloodBuffer1 = s_Data.JumpFloodBuffer2;
                s_Data.JumpFloodBuffer2 = tmp;

                step /= 2;
            }

            // Restore state
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            // Composite jumpflood buffer onto geometry buffer
            Blit(s_Data.JumpFloodBuffer1, s_Data.GeometryPass->GetSpecification().Framebuffer, s_Data.FinalJumpFloodShader, false);
        }
    }

    void SceneRenderer::CompositePass()
    {
        Renderer::BeginRenderPass(s_Data.CompositePass);

        // Move into scene setting uniform buffer
        glProgramUniform1f(s_Data.CompositeShader->GetShaderProgram(), 0, s_Data.ActiveCamera->GetExposure());

        s_Data.CompositeShader->Bind();
        s_Data.GeometryPass->GetSpecification().Framebuffer->BindColorTexture(0, 0);

        Renderer::DrawFullscreenQuad(s_Data.CompositeShader, false);

        Renderer::EndRenderPass();
    }

    void SceneRenderer::SetSelectedObject(int selectedObjectID)
    {
        s_Data.SelectedObjectID = selectedObjectID;
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

        auto size = ImGui::GetContentRegionAvail();

        if (ImGui::CollapsingHeader("Scene Settings"))
        {
            float exposure = s_Data.ActiveCamera->GetExposure();
            if (ImGui::DragFloat("Exposure", &exposure, 0.1f, 0.0f, 100.0f))
                s_Data.ActiveCamera->SetExposure(exposure);

            bool active = (bool)s_Data.UseIrradianceSkybox;
            if (ImGui::Checkbox("Irradiance Skybox", &active))
                s_Data.UseIrradianceSkybox = (uint32_t)active;
        }

        if (ImGui::CollapsingHeader("Shadow Settings"))
        {
            ImGui::DragFloat("Size", &s_Data.ShadowSetting.Size);
            ImGui::DragFloat("Near Clip", &s_Data.ShadowSetting.NearClip);
            ImGui::DragFloat("Far Clip", &s_Data.ShadowSetting.FarClip);
            ImGui::DragFloat("Max Shadow Distance", &s_Data.ShadowSetting.MaxShadowDistance);
            ImGui::DragFloat("Shadow Fade", &s_Data.ShadowSetting.ShadowFade);

            ImGui::Image((ImTextureID)s_Data.ShadowPass->GetSpecification().Framebuffer->GetDepthAttachment(), { size.x, size.x }, { 0, 1 }, { 1, 0 });
        }

        ImGui::End();
    }

    void SceneRenderer::Blit(Ref<FrameBuffer> src, Ref<FrameBuffer> destination, Ref<Shader>& shader, bool clear = true)
    {
        destination->Bind();
        if (clear)
            destination->Clear();

        shader->Bind();

        src->BindColorTexture(0, 0);
        Renderer::DrawFullscreenQuad(shader, true);

        shader->Unbind();
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

    Ref<Shader> SceneRenderer::GetEquirectToCubemapShader()
    {
        return s_Data.EquirectToCubemapShader;
    }

    Ref<Shader> SceneRenderer::GetFilterCubemapShader()
    {
        return s_Data.FilterCubemapShader;
    }

    Ref<Shader> SceneRenderer::GetComputeIrradianceMapShader()
    {
        return s_Data.ComputeIrradianceMapShader;
    }

}