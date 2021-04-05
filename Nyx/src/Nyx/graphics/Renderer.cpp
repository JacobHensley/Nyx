#include "NXpch.h"
#include "Renderer.h"
#include "Nyx/Asset/AssetManager.h"
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

namespace Nyx {
	
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
		Ref<VertexArray> MeshVA;
		Ref<IndexBuffer> MeshIB;
		glm::mat4 Transform;
	};

	enum UniformBufferID
	{
		NONE = -1, CAMERA_BUFFER = 0
	};

	struct UniformSlot
	{
		UniformSlot(UniformType type)
			:	Type(type)
		{
		}

		uint32_t Size;
		uint32_t Offset;
		UniformType Type;
	};

	struct UniformBuffer
	{
		UniformBuffer(const std::string& name, uint32_t bindingPoint, std::unordered_map<RendererUniformID, Ref<UniformSlot>>& layout)
			: Name(name), BindingPoint(bindingPoint), Size(0), BufferLayout(layout)
		{
			uint32_t offset = 0;
			for (auto& [rendererID, uniformSlot] : BufferLayout)
			{
				uniformSlot->Size = Shader::GetUniformSizeFromType(uniformSlot->Type);
				Size = offset + uniformSlot->Size;
				uniformSlot->Offset = offset;
				offset += uniformSlot->Size;
			}

			glCreateBuffers(1, &OpenGLID);
			glBindBuffer(GL_UNIFORM_BUFFER, OpenGLID);
			glBufferData(GL_UNIFORM_BUFFER, Size, nullptr, GL_DYNAMIC_DRAW);
			glBindBufferBase(GL_UNIFORM_BUFFER, BindingPoint, OpenGLID);

			BufferData = (byte*)malloc(Size);
			memset(BufferData, 0, Size);
		}

		template<typename T>
		void Set(RendererUniformID id, const T& data)
		{
			Ref<UniformSlot> uniform = BufferLayout.at(id);
			if (uniform == nullptr)
			{
				NX_CORE_WARN("Uniform {0} is not setup in uniform buffer", Name);
			}

			memcpy(BufferData + uniform->Offset, &data, uniform->Size);
		}

		const std::string Name;
		uint32_t BindingPoint;
		uint32_t OpenGLID;
		uint32_t Size;
		byte* BufferData;
		std::unordered_map<RendererUniformID, Ref<UniformSlot>> BufferLayout;
	};

	struct RendererData
	{
		Ref<Camera> ActiveCamera;
		Ref<EnvironmentMap> ActiveEnvironmentMap;

		Ref<Texture> BRDFLutTexture;
		Ref<FullscreenQaud> FullscreenQaud;
		
		std::map<MaterialRef, std::vector<SubmeshDrawCommand>> DrawList;
		std::vector<Ref<UniformBuffer>> UniformBuffers;
		std::unordered_map<RendererUniformID, std::function<void(const Ref<ShaderUniform>&)>> RendererUniformFunctions;
	};

	static RendererData s_Data;

	void Renderer::Init()
	{
		InitRendererUniformFunctions();

		s_Data.BRDFLutTexture = CreateRef<Texture>("assets/textures/Brdf_Lut.png");
		s_Data.FullscreenQaud = CreateRef<FullscreenQaud>();

		std::unordered_map<RendererUniformID, Ref<UniformSlot>> cameraBufferLayout;
		cameraBufferLayout[RendererUniformID::VIEW_PROJECTION] = CreateRef<UniformSlot>(UniformType::MAT4);
		cameraBufferLayout[RendererUniformID::INVERSE_VP] = CreateRef<UniformSlot>(UniformType::MAT4);
		s_Data.UniformBuffers.push_back(CreateRef<UniformBuffer>("r_CameraBuffer", 0, cameraBufferLayout));
	}

	void Renderer::Begin(Ref<Camera> camera, Ref<EnvironmentMap> environmentMap)
	{
		s_Data.ActiveCamera = camera;
		s_Data.ActiveEnvironmentMap = environmentMap;

		s_Data.UniformBuffers[CAMERA_BUFFER]->Set(RendererUniformID::VIEW_PROJECTION, s_Data.ActiveCamera->GetProjectionMatrix() * s_Data.ActiveCamera->GetViewMatrix());
		s_Data.UniformBuffers[CAMERA_BUFFER]->Set(RendererUniformID::INVERSE_VP, glm::inverse(s_Data.ActiveCamera->GetViewMatrix() * s_Data.ActiveCamera->GetProjectionMatrix()));
		UploadUniformBuffer(s_Data.UniformBuffers[CAMERA_BUFFER]);
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
			const std::unordered_map<RendererUniformID, Ref<ShaderUniform>>& rendererUniforms = shader->GetRendererUniforms();
			for (auto const& [rendererID, uniform] : rendererUniforms)
			{
				if (uniform->Type == UniformType::TEXTURE_2D || uniform->Type == UniformType::TEXTURE_CUBE)
				{
					s_Data.RendererUniformFunctions[rendererID](uniform);
				}
			}
			
			for (auto& submeshDC : meshList)
			{
				SubMesh& submesh = submeshDC.SubMesh;
				glm::mat4& transform = submeshDC.Transform;
				
				if (rendererUniforms.find(RendererUniformID::TRANSFORM) != rendererUniforms.end())
				{
					Ref<ShaderUniform> transformUniform = rendererUniforms.at(RendererUniformID::TRANSFORM);
					shader->SetUniformMat4(transformUniform->Name, transform);
				}

				submeshDC.MeshVA->Bind();
				submeshDC.MeshIB->Bind();

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

			s_Data.DrawList[material].push_back({ subMesh, mesh->GetVertexArray(), mesh->GetIndexBuffer(), transform * subMesh.transform });
		}
	}

	void Renderer::SubmitFullscreenQuad(Ref<Material> material)
	{
		material->Bind();
		s_Data.FullscreenQaud->Bind();

		Ref<Shader> shader = material->GetShader();
		const std::unordered_map<RendererUniformID, Ref<ShaderUniform>>& rendererUniforms = shader->GetRendererUniforms();
		for (auto const& [rendererID, uniform] : rendererUniforms)
		{
			if (uniform->Type == UniformType::TEXTURE_2D || uniform->Type == UniformType::TEXTURE_CUBE)
			{
				s_Data.RendererUniformFunctions[rendererID](uniform);
			}
		}

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
	}

	void Renderer::InitRendererUniformFunctions()
	{
		s_Data.RendererUniformFunctions[RendererUniformID::IRRADIANCE_TEXTURE] = [&](const Ref<ShaderUniform>& uniform)
		{
			AssetManager::GetByUUID<TextureCube>(s_Data.ActiveEnvironmentMap->irradianceMap.GetUUID())->Bind(uniform->Sampler);
		};
		s_Data.RendererUniformFunctions[RendererUniformID::RADIANCE_TEXTURE] = [&](const Ref<ShaderUniform>& uniform)
		{
			AssetManager::GetByUUID<TextureCube>(s_Data.ActiveEnvironmentMap->radianceMap.GetUUID())->Bind(uniform->Sampler);
		};
		s_Data.RendererUniformFunctions[RendererUniformID::BRDF_LUT] = [&](const Ref<ShaderUniform>& uniform)
		{
			s_Data.BRDFLutTexture->Bind(uniform->Sampler);
		};
	}

	void Renderer::UploadUniformBuffer(const Ref<UniformBuffer>& uniformBuffer)
	{
		glBindBuffer(GL_UNIFORM_BUFFER, uniformBuffer->OpenGLID);
		glBindBufferBase(GL_UNIFORM_BUFFER, uniformBuffer->BindingPoint, uniformBuffer->OpenGLID);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, uniformBuffer->Size, (const void*)uniformBuffer->BufferData);
	}
}