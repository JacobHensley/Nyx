#pragma once
#include "Nyx/Events/Event.h"
#include <String>

namespace Nyx {

	class Layer
	{
	public:
		Layer(const std::string& name);
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
		const std::string m_Name;
		bool m_Visible = true;
	};

}