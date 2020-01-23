#include "NXpch.h"
#include "Renderer.h"
#include "Nyx/graphics/MeshFactory.h"

namespace Nyx {

	Renderer* Renderer::s_Instance = new Renderer();
	Scene* Renderer::m_ActiveScene = nullptr;

	Renderer::Renderer()
	{
		
	}

	Renderer::~Renderer()
	{
	}

	void Renderer::Begin(Scene* scene)
	{
		m_ActiveScene = scene;
	}

	void Renderer::Flush()
	{
	}

	void Renderer::End()
	{
	}

	void Renderer::SubmitMesh(Mesh* mesh, glm::mat4 transform, Material* material)
	{
		mesh->Render(material->GetDepthTesting());
	}

}