#pragma once
#include "Nyx/scene/component/Component.h"
#include "Nyx/Script.h"

namespace Nyx {

	class ScriptComponent : public Component
	{
	public:
		ScriptComponent(Ref<Script> script);
		ScriptComponent(const String& path);

	public:
		virtual void OnAttach();
		virtual void OnDetach();

		virtual void OnUpdate();
		virtual void OnRender();

		static Component::Type* GetStaticType()
		{
			static Component::Type type = { "Script" };
			return &type;
		}

		inline virtual Component::Type* GetType() const override { return GetStaticType(); }

	public:
		Ref<Script> m_Script;
	};

}