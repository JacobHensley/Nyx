#include "NXpch.h"
#include "ScriptComponent.h"

namespace Nyx {

	ScriptComponent::ScriptComponent(AssetHandle script)
		:	m_Script(script)
	{
	}

	ScriptComponent::ScriptComponent(const String& path)
		: m_Script(AssetManager::Load<Script>(path))
	{	
	}

	void ScriptComponent::OnAttach()
	{
		m_Script.Get<Script>()->Run();
		m_Script.Get<Script>()->CallFunction<>("OnAttach");
	}

	void ScriptComponent::OnDetach()
	{
		m_Script.Get<Script>()->CallFunction<>("OnDetach");
	}

	void ScriptComponent::OnUpdate()
	{
		m_Script.Get<Script>()->CallFunction<>("OnUpdate");
	}

	void ScriptComponent::OnRender()
	{
		m_Script.Get<Script>()->CallFunction<>("OnRender");
	}

}