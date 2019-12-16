#pragma once
#include "Event.h"

namespace Nyx {

	class KeyEvent : public Event
	{
	public:
		inline int GetKeycode() const { return m_Keycode; }

	protected:
		KeyEvent(int keycode)
			: m_Keycode(keycode) {}

		int m_Keycode;
	};

	class KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(int keycode, int repeatCount)
			: KeyEvent(keycode), m_RepeatCount(repeatCount) {}

		inline int GetRepeatCount() { return m_RepeatCount; }

		String ToString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << m_Keycode << " (" << m_RepeatCount << " repeats)";
			return ss.str();
		}

		EVENT_FUNCTIONS(KeyPressed)

	private:
		int m_RepeatCount;
	};

	class KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(int keycode)
			: KeyEvent(keycode) {}

		String ToString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << m_Keycode;
			return ss.str();
		}

		EVENT_FUNCTIONS(KeyReleased)
	};

	class KeyTypedEvent : public KeyEvent
	{
	public:
		KeyTypedEvent(int keycode)
			: KeyEvent(keycode) {}

		String ToString() const override
		{
			std::stringstream ss;
			ss << "KeyTypedEvent: " << m_Keycode;
			return ss.str();
		}

		EVENT_FUNCTIONS(KeyTyped)
	};

}