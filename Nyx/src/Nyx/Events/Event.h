#pragma once
#include "NXpch.h"

namespace Nyx {

	enum EventType
	{
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		KeyPressed, KeyReleased, KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};

	#define EVENT_FUNCTIONS(type) static EventType GetStaticType() { return EventType::##type; } \
						      virtual EventType GetEventType() const override { return GetStaticType(); } \
							  virtual String GetName() const override { return #type; }

	class Event
	{
	public:
		bool m_Handled = false;
		inline bool IsHandled() { return m_Handled; }
	public:
		virtual EventType GetEventType() const = 0;
		virtual String GetName() const = 0;
		virtual String ToString() const { return GetName(); }
	};

	class EventDispatcher
	{
		template<typename T>
		using EventFn = std::function<bool(T&)>;
	public:
		EventDispatcher(Event& event)
			: m_Event(event)
		{
		}

		template<typename T>
		bool Dispatch(EventFn<T> func)
		{
			if (m_Event.GetEventType() == T::GetStaticType())
			{
				m_Event.m_Handled = func(*(T*)&m_Event);
				return true;
			}
			return false;
		}
	private:
		Event& m_Event;
	};

	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.ToString();
	}

}