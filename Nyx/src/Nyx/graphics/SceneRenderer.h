#pragma once
#include "Nyx/Core/Core.h"
#include "Nyx/Scene/Scene.h"
#include "Nyx/Graphics/API/FrameBuffer.h"
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

    class RenderPass
    {
    public:
        RenderPass()
        {
            TextureParameters parameters;
            parameters.format = TextureFormat::RGBA16F;

            FramebufferSpecification fbSpec;
            fbSpec.Scale = 1.0f;
            fbSpec.ColorAttachments.emplace_back(parameters);
            fbSpec.DepthAttachments.emplace_back(parameters);
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

    class SceneRenderer
    {
    public:
        static void Init();
        static void Begin(Scene* scene, Ref<Camera> camera);
        static void End();

        static void SubmitMesh(Ref<Mesh> mesh, glm::mat4 transform);
        static void Resize(uint width, uint height);

        static Ref<FrameBuffer> GetFinalBuffer();

        static Ref<Shader> GetPBRShader();
        static Ref<Shader> GetGlassShader();

    private:
        static void GeometryPass();
        static void CompositePass();

        static void Blit(Ref<FrameBuffer>& src, Ref<FrameBuffer>& destination, Ref<Shader>& shader, bool clear);
    };
}