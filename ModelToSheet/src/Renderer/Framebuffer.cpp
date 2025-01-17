#include "pch.h"
#include "Framebuffer.h"	
#include "Platforms/OpenGL/OpenGLFramebuffer.h"

std::shared_ptr<Framebuffer> Framebuffer::Create(uint32_t width, uint32_t height)
{
    switch (Renderer::GetAPI())
    {
    case RendererAPI::API::None:
        ASSERT(false, "RendererAPI::API::None is not supported!");
        return nullptr;
    case RendererAPI::API::OpenGL:
        return std::make_shared<OpenGLFramebuffer>(width, height);
    }

    ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}
