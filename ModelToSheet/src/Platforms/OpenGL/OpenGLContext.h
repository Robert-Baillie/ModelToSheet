#pragma once

#include "Renderer/GraphicsContext.h"

struct GLFWwindow; // Forward declaration of the GLFWwindow

class OpenGLContext : public GraphicsContext {
public:
	OpenGLContext(GLFWwindow* windowHandle);

	virtual void Init() override;
	virtual void SwapBuffers() override;

private:
	GLFWwindow* m_windowHandle;
};