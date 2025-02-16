#pragma once
#include <string>	

// From Cherno Event System: https://www.youtube.com/watch?v=xnopUoZbMEk

// Events are blocking, When the event occurs it is handled right away.

// Define the Event Types below
enum class EventType {
	None = 0,
	WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
	AppTick, AppUpdate, AppRender,
	KeyPressed, KeyReleased, KeyTyped,
	MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled,
	ModelLoadStart, ModelLoadComplete, 
	AnimationChange, AnimationKeyFrameChange,
	CameraOrbitChanged,	Export, KeyframeChange
};

// Define the Event Categories. Use BIT flags for efficiency in runtime.
enum  EventCategory {
	None = 0,
	EventCategoryApplication = BIT(0),  // 1 << 0 = 00001
	EventCategoryInput = BIT(1),		// 1 << 1 = 00010
	EventCategoryKeyboard = BIT(2),		// 1 << 2 = 00100
	EventCategoryMouse = BIT(3),		// 1 << 3 = 01000
	EventCategoryMouseButton = BIT(4),	// 1 << 4 = 10000
	EventCategoryLayerModel = BIT(5),
	EventCategoryLayerAnimation = BIT(6),
	EventCategoryLayerOther = BIT(7),
};

class Event {
	friend class EventDispatcher;
public:
	inline bool Handled() const { return m_Handled; }

	// Virtual Functions. These are to be defined in the Events themselves.
	virtual EventType GetEventType() const = 0;
	virtual const char* GetName() const = 0;
	virtual int GetCategoryFlags() const = 0;
	virtual std::string ToString() const { return GetName(); }

	// Category Check
	inline bool IsInCategory(EventCategory category) {
		return GetCategoryFlags() & category; // Can be in multiple categories use bitwise operator

		// Example:
		// myEvent.IsInCategory(EventCategory::EventCategoryTest);
		//	GetCategoryFlags() = 00110  // Defined from class.
		//	 EventCategoryTest = 00010  // The BIT define above
		//	---------------------------
		//	Result(AND) = 00010			// The result of the bitwise and is non-zero, so this is true.
	}

protected:
	bool m_Handled = false;
};