#pragma once
#include "Event.h"	

// Event Dispatcher
class EventDispatcher
{
	// Event function is a standard function, returning a bool, taking a type 'T' as a reference.
	// T in this case is any event type
	template<typename T>
	using EventFn = std::function<bool(T&)>;
public:
	EventDispatcher(Event& event) : m_Event(event) {}


	// Dispatch - Take in an event 'T' and returns a boolean
	template<typename T>
	bool Dispatch(EventFn<T> func)
	{
		// If the event you are trying to dispatch matches the event type, the callback function will be run using the event as a parameter.
		if (m_Event.GetEventType() == T::GetStaticType())
		{
			m_Event.m_Handled = func(*(T*)&m_Event);

			// func(*(T*)&m_Event); Breakdown:
			// (T*)&m_Event - cast the m_Event to a pointer of T* => treats m_Event as type T
			// *(T*)&m_Event - deference pointer of previous step, converting it back to a T&
			// func(above) - calls the function you want to dispatch

			return true;
		}
		return false;
	}

private:
	Event& m_Event;

};