#include "NXpch.h"
#include "SceneRenderer.h"
#include "Nyx/Graphics/Renderer.h"
#include "Nyx/Graphics/MeshFactory.h"
#include "Nyx/Asset/AssetManager.h"
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

namespace Nyx
{
    struct SceneRendererData
    {
        Scene* ActiveScene;
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

        s_Data.GeometryPass = CreateRef<RenderPass>();
        s_Data.CompositePass = CreateRef<RenderPass>(TextureParameters(TextureFormat::RGBA, TextureFilter::LINEAR, TextureWrap::CLAMP_TO_EDGE));
    }

    void SceneRenderer::Begin(Scene* scene, Ref<Camera> camera)
    {
        s_Data.ActiveScene = scene;
        s_Data.ActiveCamera = camera;
    }

    void SceneRenderer::End()
    {
        Renderer::Begin(s_Data.ActiveCamera, s_Data.ActiveScene->GetEnvironmentMap());

        GeometryPass();
        CompositePass();
        s_Data.RenderCommands.clear();

        s_Data.GeometryPass->Bind();
        Renderer::End();
        s_Data.GeometryPass->Unbind();
    }

    void Nyx::SceneRenderer::SubmitMesh(Ref<Mesh> mesh, glm::mat4 transform)
    {
        s_Data.RenderCommands.push_back(RenderCommand(mesh, transform));
    }

    void SceneRenderer::GeometryPass()
    {   
        s_Data.GeometryPass->Bind();

        Renderer::SubmitFullscreenQuad(s_Data.EnvironmentMaterial);

        for (RenderCommand command : s_Data.RenderCommands)
        {
            Renderer::SubmitMesh(command.Mesh, command.Transform, command.MaterialOverride);
        }

        s_Data.GeometryPass->Unbind();
    }

    void SceneRenderer::CompositePass()
    {
        s_Data.CompositePass->Bind();
        
        s_Data.CompositePass->Unbind();
    }

    void Nyx::SceneRenderer::Resize(uint width, uint height)
    {
        s_Data.GeometryPass->GetFrameBuffer()->Resize(width, height);
        s_Data.CompositePass->GetFrameBuffer()->Resize(width, height);
    }

    void SceneRenderer::Blit(Ref<FrameBuffer>& src, Ref<FrameBuffer>& destination, Ref<Shader>& shader, bool clear = true)
    {
    }

    Ref<FrameBuffer> Nyx::SceneRenderer::GetFinalBuffer()
    {
        return s_Data.GeometryPass->GetFrameBuffer();
    }

    Ref<Shader> Nyx::SceneRenderer::GetPBRShader()
    {
        return s_Data.PBRShader;
    }

    Ref<Shader> SceneRenderer::GetGlassShader()
    {
        return s_Data.GlassShader;
    }
}