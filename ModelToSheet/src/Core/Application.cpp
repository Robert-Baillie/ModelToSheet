#include "pch.h"
#include "Application.h"



Application* Application::s_Instance = nullptr;


Application::Application() {
	// Application creation. This is called when the EntryPoint.cpp file creates a new pointer for Application.
	
	// Standard check for any singleton.
	ASSERT(!s_Instance, "Application already exists.");
	s_Instance = this;

	// Set Random Seed
	srand(static_cast<unsigned int>(time(0)));

	// Create the window
	WindowProps props;
	props.Title = "Model To Sheet";
	props.Fullscreen = true;

	m_Window = std::unique_ptr<Window>(Window::Create(props));
	
	// Bind Application on event.
	 m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));


	 // Add the Layers
	 m_DockSpaceLayer = new  DockspaceLayer(); // This layer handles the imgui initialisation per renderer
	 PushLayer(m_DockSpaceLayer);
	 PushLayer(new ViewportLayer());

	 PushOverlay(new UILayer());
	 

}

Application::~Application() { }

void Application::Run() {
	while (m_Running)
	{
		// Loop through the layers and update
		for (Layer* layer : m_LayerStack)
			layer->OnUpdate();

		// ImGui
		m_DockSpaceLayer->Begin();
		for (Layer* layer : m_LayerStack)
			layer->OnImGuiRender();
		m_DockSpaceLayer->End();

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
	// Dispatching an event. Look at window close event and dispatch event if it spots one
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));
	dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::OnWindowResize));

	// Layers.
	// Descend the list and call the event. Break if it is handled.
	for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
	{
		(*--it)->OnEvent(e);
		if (e.Handled())
			break;
	}
}

/* Event Definitions */
bool Application::OnWindowClose(WindowCloseEvent& e)
{
	m_Running = false;
	return true;
}

bool Application::OnWindowResize(WindowResizeEvent& e)
{
	Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());
	

	return true;
}




