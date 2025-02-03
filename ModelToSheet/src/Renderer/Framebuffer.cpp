#include "pch.h"
#include "Framebuffer.h"	
#include "Platforms/OpenGL/OpenGLFramebuffer.h"

std::shared_ptr<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
{
    switch (Renderer::GetAPI())
    {
    case RendererAPI::API::None:
        ASSERT(false, "RendererAPI::API::None is not supported!");
        return nullptr;
    case RendererAPI::API::OpenGL:
        return std::make_shared<OpenGLFramebuffer>(spec);
    }

    ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}
