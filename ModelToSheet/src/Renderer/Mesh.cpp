#include "pch.h"
#include "Mesh.h"


#include "Renderer.h"

#include "Platforms/OpenGL/OpenGLMesh.h"

Mesh* Mesh::Create(const std::vector<Vertex>& vertices,
					const std::vector<uint32_t>& indices)
{
	switch (Renderer::GetAPI())
	{
	case RendererAPI::API::None:
		ASSERT(false, "Renderer API None is not supported!");
		return nullptr;
	case RendererAPI::API::OpenGL:
		return new OpenGLMesh(vertices, indices);
	}

	ASSERT(false, "Unknown Renderer API!");
	return nullptr;
}