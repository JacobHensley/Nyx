#pragma once
#include "NXpch.h"
#include "Nyx/Events/Event.h"

class Layer
{
public:
	Layer(const String& name);
	virtual ~Layer();
	
	virtual void OnAttach() {}
	virtual void OnDetach() {}

	virtual void Update() {}
	virtual void Render() {}
	virtual void ImGUIRender() {}
	virtual void OnEvent(Event& e) {}

	inline void Toggle() { m_Visible = !m_Visible; }
	inline bool IsVisible() const { return m_Visible; }

protected:
	bool m_Visible = true;
	const String& m_Name;
};