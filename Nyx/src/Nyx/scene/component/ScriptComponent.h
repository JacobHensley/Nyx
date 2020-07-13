#pragma once
#include "Nyx/scene/component/Component.h"
#include "Nyx/Script.h"

namespace Nyx {

	class ScriptComponent : public Component
	{
	public:
		ScriptComponent(AssetHandle script);
		ScriptComponent(const String& path);

	public:
		virtual void OnAttach();
		virtual void OnDetach();

		virtual void OnUpdate();
		virtual void OnRender();

		UUID GetUUID() { return m_Script.GetUUID(); }

		static Component::Type* GetStaticType()
		{
			static Component::Type type = { "Script" };
			return &type;
		}

		inline virtual Component::Type* GetType() const override { return GetStaticType(); }

	public:
		AssetHandle m_Script;
	};

}