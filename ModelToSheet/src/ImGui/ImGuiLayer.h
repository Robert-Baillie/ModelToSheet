#pragma once
#include "Core/Layer.h"	

#include "Events/KeyEvents.h"
#include "Events/MouseEvents.h"
#include "Events/ApplicationEvents.h"

class ImGuiLayer : public Layer
{
public: 
	ImGuiLayer();
	 ~ImGuiLayer();

	 virtual void OnAttach() override;
	 virtual void OnDetach() override;
	 virtual void OnImGuiRender() override;

	 void Begin();
	 void End();
private:
	float m_Time;
};