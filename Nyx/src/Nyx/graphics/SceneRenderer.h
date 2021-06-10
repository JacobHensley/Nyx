#pragma once
#include "Nyx/Core/Core.h"
#include "Nyx/Scene/Scene.h"
#include "Nyx/Graphics/API/FrameBuffer.h"
#include "Nyx/graphics/LightEnvironment.h"
#include "Nyx/Graphics/Shader.h"
#include "Nyx/Graphics/Mesh.h"

namespace Nyx
{
    struct RenderCommand
    {
        RenderCommand(Ref<Mesh> mesh, const glm::mat4& transform, Ref<Material> materialOverride = nullptr)
            : Mesh(mesh), Transform(transform), MaterialOverride(materialOverride)
        {
        }

        Ref<Mesh> Mesh;
        glm::mat4  Transform;
        Ref<Material> MaterialOverride;
    };
  
    class SceneRenderer
    {
    public:
        static void Init();
        static void Begin(Scene* scene, Ref<Camera> camera);
        static void End();

        static void SubmitMesh(Ref<Mesh> mesh, glm::mat4 transform);
        
        static void SetEnvironment(Ref<EnvironmentMap> environmentMap, Ref<LightEnvironment> lightEnvironment);
        static void Resize(uint32_t width, uint32_t height);

        static void OnImGuiRender();

        static Ref<FrameBuffer> GetFinalBuffer();

        static Ref<Shader> GetPBRShader();
        static Ref<Shader> GetGlassShader();
        static Ref<Shader> GetCompositeShader();
        static Ref<Shader> GetSkyboxShader();

    private:
        static void ShadowPass();
        static void GeometryPass();
        static void CompositePass();

        static void Blit(Ref<FrameBuffer>& src, Ref<FrameBuffer>& destination, Ref<Shader>& shader, bool clear);
    };
}