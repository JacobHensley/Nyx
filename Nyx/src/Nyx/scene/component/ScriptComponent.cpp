#include "NXpch.h"
#include "ScriptComponent.h"

namespace Nyx {

	ScriptComponent::ScriptComponent(Script* script)
		:	m_Script(script)
	{
	}

	ScriptComponent::ScriptComponent(const String& path)
		:	m_Script(new Script(path))
	{	
	}

	void ScriptComponent::OnAttach()
	{
		m_Script->Run();
		m_Script->CallFunction<>("OnAttach");
	}

	void ScriptComponent::OnDetach()
	{
		m_Script->CallFunction<>("OnDetach");
	}

	void ScriptComponent::OnUpdate()
	{
		m_Script->CallFunction<>("OnUpdate");
	}

	void ScriptComponent::OnRender()
	{
		m_Script->CallFunction<>("OnRender");
	}

}