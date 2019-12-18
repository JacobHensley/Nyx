#pragma once
#include "Nyx/scene/component/Component.h"
#include "Nyx/graphics/renderer/Mesh.h"

namespace Nyx {

	class MeshComponent : public Component
	{
	public:
		MeshComponent(Mesh* mesh);

	public:
		void RenderMesh(const glm::mat4& transform);
		Mesh* GetMesh() { return m_Mesh; }

		static Component::Type* GetStaticType()
		{
			static Component::Type type = { "Mesh" };
			return &type;
		}

		inline virtual Component::Type* GetType() const override { return GetStaticType(); }

	public:
		Mesh* m_Mesh;
	};

}