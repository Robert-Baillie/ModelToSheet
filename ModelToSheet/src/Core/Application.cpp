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
	 m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));


	 // Add the Layers
	 m_ImGuiLayer = new ImGuiLayer();
	 PushOverlay(m_ImGuiLayer);
	

}

Application::~Application() { }

void Application::Run() {
	while (m_Running)
	{
		// Loop through the layers and update
		for (Layer* layer : m_LayerStack)
			layer->OnUpdate();

		// ImGui
		// To Do: Seperate Update and Render into threads?
		m_ImGuiLayer->Begin();
		for (Layer* layer : m_LayerStack)
			layer->OnImGuiRender();

		m_ImGuiLayer->End();


		// Update the window
		m_Window->OnUpdate();

	}
}

void Application::PushLayer(Layer* layer)
{
	m_LayerStack.PushLayer(layer);
	layer->OnAttach();
}

void Application::PushOverlay(Layer* layer)
{
	m_LayerStack.PushOverlay(layer);
	layer->OnAttach();

}




void Application::OnEvent(Event& e)
{
	
}


