#pragma once
#include "Nyx/scene/component/Component.h"
#include "Nyx/Script.h"

namespace Nyx {

	class ScriptComponent : public Component
	{
	public:
		ScriptComponent(Script& script);

	public:
		void OnAttach();
		void OnUpdate();
		void OnRender();
		void OnDetach();

		static Component::Type* GetStaticType()
		{
			static Component::Type type = { "Script" };
			return &type;
		}

		inline virtual Component::Type* GetType() const override { return GetStaticType(); }

	public:
		Script& m_Script;
	};

}