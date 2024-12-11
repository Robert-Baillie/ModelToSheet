#include "pch.h"	
#include "Texture.h"

#include "Renderer.h"

#include "Platforms/OpenGL/OpenGLTexture.h"

Texture* Texture::Create(uint32_t width, uint32_t height, const Specification& spec, unsigned char* data)
{
	switch (Renderer::GetAPI())
	{
	case RendererAPI::API::None:
		ASSERT(false, "Renderer API None is not supported!");
		return nullptr;
	case RendererAPI::API::OpenGL:
		return new OpenGLTexture(width, height,spec, data);
	}

	ASSERT(false, "Unknown Renderer API!");
	return nullptr;
}