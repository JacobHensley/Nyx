#include "NXpch.h"
#include "Renderer.h"
#include "Nyx/graphics/MeshFactory.h"
#include "glad/glad.h"
#include "glm/gtx/matrix_decompose.hpp"

namespace Nyx {

	Renderer* Renderer::s_Instance = new Renderer();

	Renderer::Renderer()
	{
		
	}

	Renderer::~Renderer()
	{
	}

	void Renderer::Begin()
	{
	}

	void Renderer::Flush()
	{
	}

	void Renderer::End()
	{
	}

	void Renderer::SubmitMesh(Ref<Mesh> mesh, glm::mat4 transform, Ref<Material> material, Scene* scene)
	{
		if (!material->GetDepthTesting())
			glDisable(GL_DEPTH_TEST);

		mesh->GetVertexArray()->Bind();
		mesh->GetIndexBuffer()->Bind();

		std::vector<SubMesh> meshes = mesh->GetSubMeshs();
		material->Bind();
		for (SubMesh& mesh : meshes)
		{
			material->GetShader()->SetUniformMat4("r_MVP", scene->GetCamera()->GetProjectionMatrix() * scene->GetCamera()->GetViewMatrix() * transform * mesh.transform);
			material->GetShader()->SetUniformMat4("r_ModelMatrix", transform * mesh.transform);

			glDrawElementsBaseVertex(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, (void*)(mesh.indexOffset * sizeof(uint)), mesh.vertexOffset);
		}

		if (!material->GetDepthTesting())
			glEnable(GL_DEPTH_TEST);
	}

}