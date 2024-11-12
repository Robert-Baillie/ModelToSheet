#include "pch.h"
#include "RenderCommand.h"

#include "Platforms/OpenGL/OpenGLRendererAPI.h"

RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI;