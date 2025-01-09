#pragma once
#include "Window.h"

#include "ImGui/ImGuiLayer.h"

#include "Events/Event.h"
#include "Events/ApplicationEvents.h"


class Application
{
public:
	Application();
	~Application();

	void Run(); // Called once => runs the whole app loop.
	

	void OnEvent(Event& e);

	// Pushing Layers to the stack
	void PushLayer(Layer* layer);
	void PushOverlay(Layer* layer);
	
	inline static Application& Get() { return *s_Instance; } // Singleton pattern. Only every one instance of Application

	inline Window& GetWindow() { return *m_Window; }

private:
	static Application* s_Instance;

private:
	bool OnWindowClose(WindowCloseEvent& e);
	bool OnWindowResize(WindowResizeEvent& e);

private:
	bool m_Running = true;
	

	// Layers and Stack
	ImGuiLayer* m_ImGuiLayer;
	LayerStack m_LayerStack;
	
	std::unique_ptr<Window> m_Window;
};