#pragma once
#include "Event.h"

class MouseMovedEvent : public Event
{
public:
	MouseMovedEvent(float x, float y)
		: m_X(x), m_Y(y) {}

	inline float GetX() const { return m_X; }
	inline float GetY() const { return m_Y; }

	String ToString() const override
	{
		std::stringstream ss;
		ss << "MouseMovedEvent: " << m_X << ", " << m_Y;
		return ss.str();
	}

	EVENT_FUNCTIONS(MouseMoved)
private:
	float m_X, m_Y;
};

class MouseScrolledEvent : public Event
{
public:
	MouseScrolledEvent(float xOffset, float yOffset)
		: m_XOffset(xOffset), m_YOffset(yOffset) {}

	inline float GetXOffset() const { return m_XOffset; }
	inline float GetYOffset() const { return m_YOffset; }

	String ToString() const override
	{
		std::stringstream ss;
		ss << "MouseScrolledEvent: " << GetXOffset() << ", " << GetYOffset();
		return ss.str();
	}

	EVENT_FUNCTIONS(MouseScrolled)
private:
	float m_XOffset, m_YOffset;
};

class MouseButtonEvent : public Event
{
public:
	inline int GetMouseButton() { return m_Button; }

protected:
	MouseButtonEvent(int button)
		: m_Button(button) {}

	int m_Button;
};

class MouseButtonPressedEvent : public MouseButtonEvent
{
public:
	MouseButtonPressedEvent(int button)
		: MouseButtonEvent(button) {}

	String ToString() const override
	{
		std::stringstream ss;
		ss << "MouseButtonPressedEvent: " << m_Button;
		return ss.str();
	}

	EVENT_FUNCTIONS(MouseButtonPressed)
};

class MouseButtonReleasedEvent : public MouseButtonEvent
{
public:
	MouseButtonReleasedEvent(int button)
		: MouseButtonEvent(button) {}

	String ToString() const override
	{
		std::stringstream ss;
		ss << "MouseButtonReleasedEvent: " << m_Button;
		return ss.str();
	}

	EVENT_FUNCTIONS(MouseButtonReleased)
};