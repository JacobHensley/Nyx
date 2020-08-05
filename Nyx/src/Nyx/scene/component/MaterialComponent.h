#pragma once
#include "Nyx/scene/component/Component.h"
#include "Nyx/graphics/renderer/Mesh.h"
#include "Nyx/graphics/renderer/Material.h"
#include "Nyx/AssetManager.h"

namespace Nyx {

	class MaterialComponent : public Component
	{
	public:
		MaterialComponent(AssetHandle material);

	public:
		Ref<Material> GetMaterial() { return m_Material.Get<Material>(); }

		UUID GetUUID() { return m_Material.GetUUID(); }

		static Component::Type* GetStaticType()
		{
			static Component::Type type = { "Material" };
			return &type;
		}

		inline virtual Component::Type* GetType() const override { return GetStaticType(); }

	public:
		AssetHandle m_Material;
	};

}