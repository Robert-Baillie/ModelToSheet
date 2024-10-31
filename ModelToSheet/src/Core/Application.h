#pragma once
#include "events/Event.h"

class Application
{
public:
	Application();
	~Application();

	void Run(); // Called once => runs the whole app loop.
	

	void OnEvent(Event& e);

	//void PushLayer(Layer* layer);
	//void PushOverlay(Layer* layer);
	
	inline static Application& Get() { return *s_Instance; } // Singleton pattern. Only every one instance of Application

	//inline Window& GetWindow() { return *m_Window; }

private:
	static Application* s_Instance;

private:
	bool m_Running = true;

	// LayerStack m_LayerStack;
	
	//std::unique_ptr<Window> m_Window;
};