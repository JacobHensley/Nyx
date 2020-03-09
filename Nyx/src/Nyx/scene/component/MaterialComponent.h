#pragma once
#include "Nyx/scene/component/Component.h"
#include "Nyx/graphics/renderer/Mesh.h"
#include "Nyx/graphics/renderer/Material.h"

namespace Nyx {

	class MaterialComponent : public Component
	{
	public:
		MaterialComponent(Ref<Material> material);

	public:
		Ref<Material> GetMaterial() { return m_Material; }


		static Component::Type* GetStaticType()
		{
			static Component::Type type = { "Material" };
			return &type;
		}

		inline virtual Component::Type* GetType() const override { return GetStaticType(); }

	public:
		Ref<Material> m_Material;
	};

}