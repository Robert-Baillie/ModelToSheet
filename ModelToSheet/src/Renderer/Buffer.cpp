#include "pch.h"
#include "Buffer.h"

#include "Renderer.h"

#include "Platforms/OpenGL/OpenGLBuffer.h"

// Create a buffer pointer depending on the Render API we are using.
std::shared_ptr<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size)
{
	switch (Renderer::GetAPI())
	{
	case RendererAPI::API::None:
		ASSERT(false, "RendererAPI::APINone is currently not supported!");
		return nullptr;
	case RendererAPI::API::OpenGL:
		return std::make_shared<OpenGLVertexBuffer>(vertices, size); 
	}

	ASSERT(false, "Unknown renderer API");
	return nullptr;
}

std::shared_ptr<IndexBuffer> IndexBuffer::Create(uint32_t* indicies, uint32_t size)
{
	switch (Renderer::GetAPI())
	{
	case RendererAPI::API::None:
		ASSERT(false, "RendererAPI::API::None is currently not supported!");
		return nullptr;
	case RendererAPI::API::OpenGL:
		return std::make_shared<OpenGLIndexBuffer>(indicies, size);
	}

	ASSERT(false, "Unknown renderer API");
	return nullptr;
}
