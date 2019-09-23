#pragma once
#include "Nyx/scene/component/Component.h"
#include "Nyx/graphics/renderer/Mesh.h"

namespace Nyx {

	class MeshComponent : public Component
	{
	public:
		Mesh m_Mesh;

	public:
		MeshComponent(const Mesh& mesh);

		const Mesh& GetMesh() { return m_Mesh; }
		void RenderMesh(const glm::mat4& transform);

		static Component::Type* GetStaticType()
		{
			static Component::Type type = { "Mesh" };
			return &type;
		}

		inline virtual Component::Type* GetType() const override { return GetStaticType(); }

	};

}