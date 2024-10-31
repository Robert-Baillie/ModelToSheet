#include "pch.h"
#include "Application.h"

Application* Application::s_Instance = nullptr;


Application::Application() {
	// Application creation. This is called when the EntryPoint.cpp file creates a new pointer for Application.
	
	// Standard check for any singleton.
	ASSERT(!s_Instance, "Application already exists.")
	s_Instance = this;

	// Set Random Seed
	srand(static_cast<unsigned int>(time(0)));

	// Create the window
	m_Window = std::unique_ptr<Window>(Window::Create());
	
	// Bind Application on event.
	// m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));


	

}

Application::~Application() { }

void Application::Run() {
	while (m_Running)
	{

		TRACE_LOG("Should not be tracing this every frame.");

		m_Window->OnUpdate();

	}
}

void Application::OnEvent(Event& e)
{
}


