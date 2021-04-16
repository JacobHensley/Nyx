#include "NXpch.h"
#include "Renderer.h"
#include "Nyx/Asset/AssetManager.h"
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

namespace Nyx {

	namespace UniformBuffer {

		enum UniformBufferBinding
		{
			NONE = -1, CAMERA_BUFFER, LIGHT_BUFFER
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
			PointLight PointLight;
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
		Ref<Camera> ActiveCamera;
		Ref<EnvironmentMap> ActiveEnvironmentMap;

		Ref<Texture> BRDFLutTexture;
		Ref<FullscreenQaud> FullscreenQaud;
		
		UniformBuffer::CameraBuffer CameraBuffer;
		uint32_t CameraBufferID = 0;

		UniformBuffer::LightBuffer LightBuffer;
		uint32_t LightBufferID = 1;

		std::map<MaterialRef, std::vector<SubmeshDrawCommand>> DrawList;
		std::unordered_map<std::string, std::function<void(const ShaderResource&)>> RendereResourceFunctions;
	};

	static RendererData s_Data;

	void Renderer::Init()
	{
		InitRendereResourceFunctions();

		GenerateUniformBuffer(s_Data.CameraBufferID, UniformBuffer::UniformBufferBinding::CAMERA_BUFFER, sizeof(s_Data.CameraBuffer));
		GenerateUniformBuffer(s_Data.LightBufferID, UniformBuffer::UniformBufferBinding::LIGHT_BUFFER, sizeof(s_Data.LightBuffer));

		s_Data.BRDFLutTexture = CreateRef<Texture>("assets/textures/Brdf_Lut.png");
		s_Data.FullscreenQaud = CreateRef<FullscreenQaud>();
	}

	void Renderer::Begin(Ref<Camera> camera, Ref<EnvironmentMap> environmentMap, Ref<LightEnvironment> lightEnvironment)
	{
		s_Data.ActiveCamera = camera;
		s_Data.ActiveEnvironmentMap = environmentMap;

		s_Data.CameraBuffer.ViewProjection = s_Data.ActiveCamera->GetProjectionMatrix() * s_Data.ActiveCamera->GetViewMatrix();
		s_Data.CameraBuffer.InverseViewProjection = glm::inverse(s_Data.ActiveCamera->GetProjectionMatrix() * s_Data.ActiveCamera->GetViewMatrix());
		s_Data.CameraBuffer.CameraPosition = s_Data.ActiveCamera->GetPosition();
		UploadUniformBuffer(s_Data.CameraBufferID, UniformBuffer::UniformBufferBinding::CAMERA_BUFFER, sizeof(s_Data.CameraBuffer), &s_Data.CameraBuffer);

		s_Data.LightBuffer.DirectionalLight = lightEnvironment->GetDirectionalLight();
		s_Data.LightBuffer.PointLight = lightEnvironment->GetPointLight();
		UploadUniformBuffer(s_Data.LightBufferID, UniformBuffer::UniformBufferBinding::LIGHT_BUFFER, sizeof(s_Data.LightBuffer), &s_Data.LightBuffer);
	}

	void Renderer::End()
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
			const std::unordered_map<std::string,ShaderResource>& resources = shader->GetResources();
			for (auto const& [name, resource] : resources)
			{
				if (resource.Name.find("r_") != std::string::npos)
					s_Data.RendereResourceFunctions[name](resource);
			}
			
			const std::unordered_map<std::string, ShaderUniform>& rendererUniforms = shader->GetRendererUniforms();
			for (auto& submeshDC : meshList)
			{
				SubMesh& submesh = submeshDC.SubMesh;
				glm::mat4& transform = submeshDC.Transform;

				if (rendererUniforms.find("u_Renderer.r_Transform") != rendererUniforms.end())
				{
					ShaderUniform transformUniform = rendererUniforms.at("u_Renderer.r_Transform");
					shader->SetUniformMat4(transformUniform.Name, transform);
				}

				submeshDC.MeshVA->Bind();
				submeshDC.MeshIB->Bind();

				glEnable(GL_DEPTH_TEST);
				glDrawElementsBaseVertex(GL_TRIANGLES, submesh.indexCount, GL_UNSIGNED_INT, (void*)(submesh.indexOffset * sizeof(uint)), submesh.vertexOffset);
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
				material = materials[subMesh.materialIndex];
			}

			s_Data.DrawList[material].push_back({ subMesh, transform * subMesh.transform, mesh->GetVertexArray(), mesh->GetIndexBuffer() });
		}
	}

	void Renderer::SubmitFullscreenQuad(Ref<Material> material)
	{
		material->Bind();
		s_Data.FullscreenQaud->Bind();

		Ref<Shader> shader = material->GetShader();
		const std::unordered_map<std::string,ShaderResource>& resources = shader->GetResources();
		for (auto const& [name, resource] : resources)
		{
			s_Data.RendereResourceFunctions[name](resource);
		}

		if (material->GetMaterialFlags() & (int)MaterialFlags::DISABLE_DEPTH_TEST)
			glDisable(GL_DEPTH_TEST);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
	}

	void Renderer::InitRendereResourceFunctions()
	{
		s_Data.RendereResourceFunctions["r_IrradianceTexture"] = [&](const ShaderResource& uniform)
		{
			AssetManager::GetByUUID<TextureCube>(s_Data.ActiveEnvironmentMap->irradianceMap.GetUUID())->Bind(uniform.TextureUnit);
		};
		s_Data.RendereResourceFunctions["r_RadianceTexture"] = [&](const ShaderResource& uniform)
		{
			AssetManager::GetByUUID<TextureCube>(s_Data.ActiveEnvironmentMap->radianceMap.GetUUID())->Bind(uniform.TextureUnit);
		};
		s_Data.RendereResourceFunctions["r_BRDFLutTexture"] = [&](const ShaderResource& uniform)
		{
			s_Data.BRDFLutTexture->Bind(uniform.TextureUnit);
		};
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
		glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, bufferID);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, size, data);
	}
}