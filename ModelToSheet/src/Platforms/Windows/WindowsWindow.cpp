#include "pch.h"
#include "WindowsWindow.h"

#include "Events/KeyEvents.h"
#include "Events/MouseEvents.h"
#include "Events/ApplicationEvents.h"

#include "Platforms/OpenGL/OpenGLContext.h"

static bool s_GLFWInitialised = false;

static void GLFWErrorCallback(int error, const char* desc)
{
	ERROR_LOG("GLFW Error ({0}) {1}: ", error, desc);
}

// Static function needed for all window implementations
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

	// True fullscreen below.
		/// GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
		/// const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
		/// m_Window = glfwCreateWindow(mode->width, mode->height, m_Data.Title.c_str(), primaryMonitor, nullptr);

	m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);
	if(props.Fullscreen) glfwMaximizeWindow(m_Window);

	// Create GL Context
	 m_Context = new OpenGLContext(m_Window);
	 m_Context->Init();

	 // Enable Depth Testing
	 glEnable(GL_DEPTH_TEST);   // Enable depth testing
	 glDepthFunc(GL_LESS);      // Specify the depth comparison function


	// Set the GLFW User Pointer.
	glfwSetWindowUserPointer(m_Window, &m_Data);
	SetVSync(true);

	// Setting the GLFW Callbacks
	// Using a Lambda function so we do not need to define any more methods
	glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) 
	{

		// We set up "user pointer" of the GLFWwindow to be m_Data, so we can now grab it from the GLFWwindow we pass through this lambda function
		// *(....) to dereference the (WindowData*) cast.
		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
		data.Width = width;
		data.Height = height;


		// Create the event and set the callback
		WindowResizeEvent event(width, height);
		// TRACE_LOG(event.ToString());
		data.EventCallback(event);

	});
	

	glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
	{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			WindowCloseEvent event;
			// TRACE_LOG(event.ToString());
			data.EventCallback(event);
	});


	glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode) {
		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

		KeyTypedEvent event(keycode);
		// TRACE_LOG(event.ToString());
		data.EventCallback(event);
	});


	glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

		switch (action)
		{
		case GLFW_PRESS:
		{
			KeyPressedEvent event(key, 0);
			// TRACE_LOG(event.ToString());
			data.EventCallback(event);
			break;

		}
		case GLFW_RELEASE:
		{
			KeyReleasedEvent event(key);
			// TRACE_LOG(event.ToString());
			data.EventCallback(event);
			break;
		}
		case GLFW_REPEAT:
		{
			KeyPressedEvent event(key, 1);
			// TRACE_LOG(event.ToString());
			data.EventCallback(event);
			break;
		}
		}
	});


	glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
	{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
			case GLFW_PRESS:
			{
				MouseButtonPressedEvent event(button);
				// TRACE_LOG(event.ToString());
				data.EventCallback(event);
				break;

			}
			case GLFW_RELEASE:
			{
				MouseButtonReleasedEvent event(button);
				// TRACE_LOG(event.ToString());
				data.EventCallback(event);
				break;
			}
			}
	});

	glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset) 
	{
		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

		MouseScrolledEvent event(float(xOffset), (float)yOffset);
		// TRACE_LOG(event.ToString());
		data.EventCallback(event);

	});

	glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
	{
		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

		MouseMovedEvent event(float(xPos), (float)yPos);
		// TRACE_LOG(event.ToString());
		data.EventCallback(event);


	});
}

void WindowsWindow::OnUpdate()
{
	double currentTime = glfwGetTime();
	m_DeltaTime = currentTime - m_LastFrameTime;
	m_LastFrameTime = currentTime;


	glfwPollEvents();
	m_Context->SwapBuffers();
}

void WindowsWindow::SetVSync(bool enabled)
{
	if (enabled)
		glfwSwapInterval(1);
	else
		glfwSwapInterval(0);

	m_Data.VSync = enabled;
}

bool WindowsWindow::IsVSync() const
{
	return m_Data.VSync;
}



void WindowsWindow::Shutdown()
{
	glfwDestroyWindow(m_Window);
}
