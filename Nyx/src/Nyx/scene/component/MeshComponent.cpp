#include "NXpch.h"
#include "MeshComponent.h"

namespace Nyx {

	MeshComponent::MeshComponent(Mesh* mesh)
		: m_Mesh(mesh)
	{
	}

	void MeshComponent::RenderMesh(const glm::mat4& transform)
	{
		m_Mesh->Render(true);
	}

}