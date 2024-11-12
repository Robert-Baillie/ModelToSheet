#include "pch.h"	
#include "VertexArray.h"

#include "Renderer.h"

#include "Platforms/OpenGL/OpenGLVertexArray.h"

VertexArray* VertexArray::Create()
{
	switch (Renderer::GetAPI())
	{
	case RendererAPI::API::None:
		ASSERT(false, "Renderer API None is not supported!");
		return nullptr;
	case RendererAPI::API::OpenGL:
		return new OpenGLVertexArray();
	}

	ASSERT(false, "Unknown Renderer API!");
	return nullptr;
}