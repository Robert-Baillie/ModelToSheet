#include "pch.h"
#include "OpenGLContext.h"

#include <glad/glad.h>

#include <glfw/glfw3.h>

OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
	: m_windowHandle(windowHandle)
{
	ASSERT(windowHandle, "Window Handle in GL Context is null.");
}

void OpenGLContext::Init()
{
	glfwMakeContextCurrent(m_windowHandle);

	// GLAD Loading
	int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	ASSERT(status, "OpenGL Context Failed to initialise GLAD!");

	INFO_LOG("OpenGL Info:");
	INFO_LOG("	Vendor: {0}", (const char*)glGetString(GL_VENDOR));
	INFO_LOG("	Renderer: {0}", (const char*)glGetString(GL_RENDERER));
	INFO_LOG("	Version {0}", (const char*)glGetString(GL_VERSION));

}

void OpenGLContext::SwapBuffers()
{
	glfwSwapBuffers(m_windowHandle);
}
