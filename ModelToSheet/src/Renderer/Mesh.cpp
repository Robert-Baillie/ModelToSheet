#include "pch.h"
#include "Mesh.h"
#include "Renderer.h"
#include "Platforms/OpenGL/OpenGLMesh.h"

std::shared_ptr<Mesh> Mesh::Create(const std::vector<Vertex>& vertices,
    const std::vector<uint32_t>& indices)
{
    switch (Renderer::GetAPI())
    {
    case RendererAPI::API::None:
        ASSERT(false, "RendererAPI::None is currently not supported!");
        return nullptr;
    case RendererAPI::API::OpenGL:
        return std::make_shared<OpenGLMesh>(vertices, indices);
    }

    ASSERT(false, "Unknown renderer API");
    return nullptr;
}