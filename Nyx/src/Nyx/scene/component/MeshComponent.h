#pragma once
#include "Nyx/scene/component/Component.h"
#include "Nyx/graphics/renderer/Mesh.h"
#include "Nyx/AssetManager.h"

namespace Nyx {

	class MeshComponent : public Component
	{
	public:
		MeshComponent(AssetHandle mesh);

	public:
		AssetHandle Get() { return m_Mesh; }
		void Set(AssetHandle handle) { m_Mesh = handle; }

		Ref<Mesh> GetMesh() { return m_Mesh.Get<Mesh>(); }

		UUID GetUUID() { return m_Mesh.GetUUID(); }

		static Component::Type* GetStaticType()
		{
			static Component::Type type = { "Mesh" };
			return &type;
		}

		inline virtual Component::Type* GetType() const override { return GetStaticType(); }

	public:
		AssetHandle m_Mesh;
	};

}