#pragma once

#include "Events/Event.h"

class Layer {
public:
	Layer(const std::string& name = "Layer") 
		: m_DebugName(name) {}

	virtual ~Layer() {};

	virtual void OnAttach() {}
	virtual void OnDetach() {}
	virtual void OnUpdate() {}
	virtual void OnImGuiRender() {} // Available on Windows, Max and Linux.
	virtual void OnEvent(Event& event) {}

	// Debug name is as it says. Should not be used to find layers
	inline const std::string& GetName() const { return m_DebugName; }

protected:
	std::string m_DebugName;
};