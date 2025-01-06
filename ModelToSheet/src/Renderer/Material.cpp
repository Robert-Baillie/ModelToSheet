#include "pch.h"	
#include "Material.h"

#include "Renderer.h"

#include "Platforms/OpenGL/OpenGLMaterial.h"

Material* Material::Create(const MaterialProperties& props)
{
	switch (Renderer::GetAPI())
	{
	case RendererAPI::API::None:
		ASSERT(false, "Renderer API None is not supported!");
		return nullptr;
	case RendererAPI::API::OpenGL:
		return new OpenGLMaterial(props);
	}

	ASSERT(false, "Unknown Renderer API!");
	return nullptr;
}