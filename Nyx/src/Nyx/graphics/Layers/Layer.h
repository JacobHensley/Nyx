#pragma once
#include "NXpch.h"
#include "Nyx/Events/Event.h"

namespace Nyx {

	class Layer
	{
	public:
		Layer(const String& name);
		virtual ~Layer();

	public:
		virtual void OnAttach() {}
		virtual void OnDetach() {}

		virtual void Update() {}
		virtual void Render() {}

		virtual void ImGUIRender() {}

		virtual void OnEvent(Event& e) {}

		inline void Toggle() { m_Visible = !m_Visible; }
		inline bool IsVisible() const { return m_Visible; }

	protected:

		const String& m_Name;
		bool m_Visible = true;
	};

}