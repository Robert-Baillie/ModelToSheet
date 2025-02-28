#pragma once
#include "Event.h"

// Base class for all Key Events
class KeyEvent : public Event {
public:
	inline int GetKeyCode() const { return m_KeyCode;  }

	EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)

protected:
	KeyEvent(int keycode) : m_KeyCode(keycode) {}
	
	int m_KeyCode;
};



class KeyPressedEvent : public KeyEvent
{
public:
	KeyPressedEvent(const int keycode, bool isRepeat = false)
		: KeyEvent(keycode), m_IsRepeat(isRepeat) {}

	bool IsRepeat() const { return m_IsRepeat; }

	std::string ToString() const override
	{
		std::stringstream ss;
		ss << "KeyPressedEvent: " << m_KeyCode << " (repeat = " << m_IsRepeat << ")";
		return ss.str();
	}

	EVENT_CLASS_TYPE(KeyPressed)
private:
	bool m_IsRepeat;
};




class KeyReleasedEvent : public KeyEvent
{
public:
	KeyReleasedEvent(const int keycode)
		: KeyEvent(keycode) {}

	std::string ToString() const override
	{
		std::stringstream ss;
		ss << "KeyReleasedEvent: " << m_KeyCode;
		return ss.str();
	}

	EVENT_CLASS_TYPE(KeyReleased)
};





class KeyTypedEvent : public KeyEvent
{
public:
	KeyTypedEvent(const int keycode)
		: KeyEvent(keycode) {}

	std::string ToString() const override
	{
		std::stringstream ss;
		ss << "KeyTypedEvent: " << m_KeyCode;
		return ss.str();
	}

	EVENT_CLASS_TYPE(KeyTyped)
};

