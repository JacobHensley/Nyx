#include "NXpch.h"
#include "Renderer.h"
#include "Nyx/Asset/AssetManager.h"
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

namespace Nyx {

	namespace RenderResourceBindings
	{
		const uint32_t BRDFLut = 0;
		const uint32_t RadianceMap = 1;
		const uint32_t IrradianceMap = 2;
		const uint32_t ShadowMap = 3;
	};

	namespace UniformBuffer {

		enum UniformBufferBinding
		{
			NONE = -1, CAMERA_BUFFER, LIGHT_BUFFER, SHADOW_BUFFER
		};

		struct CameraBuffer
		{
			glm::mat4 ViewProjection;
			glm::mat4 InverseViewProjection;
			glm::vec3 CameraPosition;
		};

		struct LightBuffer
		{
			DirectionalLight DirectionalLight;
			PointLight PointLights[8];
		};

		struct ShadowBuffer
		{
			glm::mat4 ShadowMapViewProjection;
		};
	} 

	struct MaterialRef
	{
		Ref<Nyx::Material> Material;

		MaterialRef(const Ref<Nyx::Material>& material)
			: Material(material)
		{
		}

		bool operator<(const MaterialRef& other) const
		{
			return *Material.get() < *other.Material.get();
		}

		Nyx::Material& operator->()
		{
			return *Material.get();
		}
	};

	struct SubmeshDrawCommand
	{
		Nyx::SubMesh SubMesh;
		glm::mat4 Transform;
		Ref<VertexArray> MeshVA;
		Ref<IndexBuffer> MeshIB;
	};

	struct RendererData
	{
		Ref<RenderPass> ActiveRenderPass;
		Ref<Camera> ActiveCamera;
		Ref<EnvironmentMap> ActiveEnvironmentMap;

		Ref<Texture> BRDFLutTexture;
		Ref<TextureCube> DefaultSkyboxTextureRadiance;
		Ref<TextureCube> DefaultSkyboxTextureIrradiance;
		Ref<FullscreenQaud> FullscreenQaud;
		
		UniformBuffer::CameraBuffer CameraBuffer;
		uint32_t CameraBufferID = -1;

		UniformBuffer::LightBuffer LightBuffer;
		uint32_t LightBufferID = -1;

		UniformBuffer::ShadowBuffer ShadowBuffer;
		uint32_t ShadowBufferID = -1;

		std::map<MaterialRef, std::vector<SubmeshDrawCommand>> DrawList; // Map::<sortkey, Map::<Shader, Map::<Material, drawlist>>>
		std::unordered_map<std::string, std::function<void(const ShaderResource&)>> RendereResourceFunctions;
	};

	static RendererData s_Data;

	void Renderer::Init()
	{
		GenerateUniformBuffer(s_Data.CameraBufferID, UniformBuffer::UniformBufferBinding::CAMERA_BUFFER, sizeof(s_Data.CameraBuffer));
		GenerateUniformBuffer(s_Data.LightBufferID, UniformBuffer::UniformBufferBinding::LIGHT_BUFFER, sizeof(s_Data.LightBuffer));
		GenerateUniformBuffer(s_Data.ShadowBufferID, UniformBuffer::UniformBufferBinding::SHADOW_BUFFER, sizeof(s_Data.ShadowBuffer));

		s_Data.DefaultSkyboxTextureRadiance = CreateRef<TextureCube>("assets/Textures/Kiara Dawn.hdr");
		s_Data.DefaultSkyboxTextureIrradiance = s_Data.DefaultSkyboxTextureRadiance->CalculateIrradianceMap();

		s_Data.BRDFLutTexture = CreateRef<Texture>("assets/textures/Brdf_Lut.png");
		s_Data.FullscreenQaud = CreateRef<FullscreenQaud>();
	}

	void Renderer::BeginRenderPass(Ref<RenderPass> renderPass)
	{
		s_Data.ActiveRenderPass = renderPass;
		Ref<FrameBuffer> framebuffer = renderPass->GetSpecification().Framebuffer;
		GLuint framebufferID = framebuffer->GetFrameBufferID();

		glViewport(0, 0, framebuffer->GetWidth(), framebuffer->GetHeight());
		glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void Renderer::EndRenderPass()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Renderer::BeginScene(Ref<Camera> camera)
	{
		s_Data.ActiveCamera = camera;

		s_Data.CameraBuffer.ViewProjection = s_Data.ActiveCamera->GetProjectionMatrix() * s_Data.ActiveCamera->GetViewMatrix();
		s_Data.CameraBuffer.InverseViewProjection = glm::inverse(s_Data.ActiveCamera->GetProjectionMatrix() * s_Data.ActiveCamera->GetViewMatrix());
		s_Data.CameraBuffer.CameraPosition = s_Data.ActiveCamera->GetPosition();
		UploadUniformBuffer(s_Data.CameraBufferID, UniformBuffer::UniformBufferBinding::CAMERA_BUFFER, sizeof(s_Data.CameraBuffer), &s_Data.CameraBuffer);

		s_Data.BRDFLutTexture->Bind(RenderResourceBindings::BRDFLut);
	}

	void Renderer::EndScene()
	{
		FlushDrawList();
	}

	void Renderer::SetEnvironment(Ref<EnvironmentMap> environmentMap, Ref<LightEnvironment> lightEnvironment)
	{
		if (environmentMap->radianceMap != 0)
		{
			Ref<TextureCube> radiance = AssetManager::GetByUUID<TextureCube>(environmentMap->radianceMap.GetUUID());
			radiance->Bind(RenderResourceBindings::RadianceMap);
		}
		else
		{
			s_Data.DefaultSkyboxTextureRadiance->Bind(RenderResourceBindings::RadianceMap);
		}

		if (environmentMap->irradianceMap != 0)
		{
			Ref<TextureCube> irradiance = AssetManager::GetByUUID<TextureCube>(environmentMap->irradianceMap.GetUUID());
			irradiance->Bind(RenderResourceBindings::IrradianceMap);
		}
		else
		{
			s_Data.DefaultSkyboxTextureIrradiance->Bind(RenderResourceBindings::IrradianceMap);
		}

		if (lightEnvironment->GetDirectionalLights().size() > 0)
			s_Data.LightBuffer.DirectionalLight = *lightEnvironment->GetDirectionalLights()[0];
		else
			s_Data.LightBuffer.DirectionalLight = DirectionalLight();

		for (int i = 0; i < 8; i++)
		{
			if (lightEnvironment->GetPointLights().size() > i)
				s_Data.LightBuffer.PointLights[i] = *lightEnvironment->GetPointLights()[i];
			else
				s_Data.LightBuffer.PointLights[i] = PointLight();
		}

		UploadUniformBuffer(s_Data.LightBufferID, UniformBuffer::UniformBufferBinding::LIGHT_BUFFER, sizeof(s_Data.LightBuffer), &s_Data.LightBuffer);
	}

	void Renderer::SetShadowMap(const glm::mat4 viewProjection, uint32_t ID)
	{
		glBindTextureUnit(RenderResourceBindings::ShadowMap, ID);

		s_Data.ShadowBuffer.ShadowMapViewProjection = viewProjection;
		UploadUniformBuffer(s_Data.ShadowBufferID, UniformBuffer::UniformBufferBinding::SHADOW_BUFFER, sizeof(s_Data.ShadowBuffer), &s_Data.ShadowBuffer);
	}

	void Renderer::FlushDrawList()
	{
		for (auto& [materialRef, meshList] : s_Data.DrawList)
		{
			Ref<Material> material = materialRef.Material;
			glm::vec3 cameraPosition = s_Data.ActiveCamera->GetPosition();

			std::sort(meshList.begin(), meshList.end(), [material, cameraPosition](auto& a, auto& b)
			{
				float MeshADistance = glm::length(cameraPosition - glm::vec3(a.Transform[3]));
				float MeshBDistance = glm::length(cameraPosition - glm::vec3(b.Transform[3]));
				if (!material->IsOpaque())
					return MeshADistance > MeshBDistance;

				return MeshADistance < MeshBDistance;
			});

			material->Bind();

			Ref<Shader> shader = material->GetShader();
			
			const std::unordered_map<std::string, ShaderUniform>& rendererUniforms = shader->GetRendererUniforms();
			for (auto& submeshDC : meshList)
			{
				SubMesh& submesh = submeshDC.SubMesh;
				glm::mat4& transform = submeshDC.Transform; // Entity transform

				if (rendererUniforms.find("r_Renderer.Transform") != rendererUniforms.end())
				{
					ShaderUniform transformUniform = rendererUniforms.at("r_Renderer.Transform");
					shader->SetUniformMat4(transformUniform.Name, transform * submesh.Transform);
				}

				submeshDC.MeshVA->Bind();
				submeshDC.MeshIB->Bind();

				glEnable(GL_DEPTH_TEST); // TEMP
				glDrawElementsBaseVertex(GL_TRIANGLES, submesh.IndexCount, GL_UNSIGNED_INT, (void*)(submesh.IndexOffset * sizeof(uint)), submesh.VertexOffset);
			}
		}

		s_Data.DrawList.clear();
	}

	void Renderer::SubmitMesh(Ref<Mesh> mesh, glm::mat4 transform, Ref<Material> materialOverride)
	{
		auto& subMeshes = mesh->GetSubMeshs();
		const auto& materials = mesh->GetMaterials();

		for (SubMesh& subMesh : subMeshes)
		{
			Ref<Material> material;
			if (materialOverride != nullptr)
			{
				material = materialOverride;
			}
			else
			{
				material = materials[subMesh.MaterialIndex];
			}

			s_Data.DrawList[material].push_back({ subMesh, transform, mesh->GetVertexArray(), mesh->GetIndexBuffer() });
		}
	}

	void Renderer::DrawFullscreenQuad(Ref<Shader> shader, bool depthTesting)
	{
		shader->Bind();
		s_Data.FullscreenQaud->Bind();

		if (!depthTesting)
			glDisable(GL_DEPTH_TEST);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
	}

	void Renderer::GenerateUniformBuffer(uint32_t& bufferID, uint32_t bindingPoint, uint32_t size)
	{
		glCreateBuffers(1, &bufferID);
		glBindBuffer(GL_UNIFORM_BUFFER, bufferID);
		glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, bufferID);
	}

	void Renderer::UploadUniformBuffer(uint32_t bufferID, uint32_t bindingPoint, uint32_t size, const void* data)
	{
		glBindBuffer(GL_UNIFORM_BUFFER, bufferID);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, size, data);
	}
}