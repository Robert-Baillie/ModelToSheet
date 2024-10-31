#include "pch.h"
#include "WindowsWindow.h"



static bool s_GLFWInitialised = false;

static void GLFWErrorCallback(int error, const char* desc)
{
	ERROR_LOG("GLFW Error ({0}) {1}: ", error, desc);
}

// Static function needed for all implementation.
Window* Window::Create(const WindowProps& props)
{
	return new WindowsWindow(props);
}

// Initialisation of windows
WindowsWindow::WindowsWindow(const WindowProps& props)
{
	Init(props);
}

WindowsWindow::~WindowsWindow()
{
	Shutdown();
}

void WindowsWindow::Init(const WindowProps& props)
{
	m_Data.Title = props.Title;
	m_Data.Width = props.Width;
	m_Data.Height = props.Height;

	INFO_LOG("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

	if (!s_GLFWInitialised)
	{
		// TO DO, GLFWTerminate on system shutdown. Only asserting in debug build
		int success = glfwInit();
		ASSERT(success, "Could not initialise GLFW!");
		glfwSetErrorCallback(GLFWErrorCallback);
		s_GLFWInitialised = true;
	}

	m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);


	// Create GL Context
	 // m_Context = new OpenGLContext(m_Window);
	 // m_Context->Init();

	// Set the GLFW User Pointer.
	glfwSetWindowUserPointer(m_Window, &m_Data);
	SetVSync(true);

}

void WindowsWindow::OnUpdate()
{
	glfwPollEvents();
	// m_Context->SwapBuffers();
}

void WindowsWindow::SetVSync(bool enabled)
{
}

bool WindowsWindow::IsVSync() const
{
	return false;
}



void WindowsWindow::Shutdown()
{
}
