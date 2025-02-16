#include "pch.h"
#include "OpenGLFramebuffer.h"
#include "OpenGLTexture.h"

#include <glad/glad.h>	

OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& spec)
	: m_Specification(spec)
{
	RecalculateFramebuffer();
	
}

OpenGLFramebuffer::~OpenGLFramebuffer()
{
	glDeleteFramebuffers(1, &m_RendererID);
	glDeleteTextures(1, &m_ColorAttachment);
	glDeleteRenderbuffers(1, &m_DepthAttatchment);
}

void OpenGLFramebuffer::Bind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
    glViewport(0, 0, m_Specification.Width, m_Specification.Height); 
}

void OpenGLFramebuffer::Unbind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void OpenGLFramebuffer::Resize(uint32_t width, uint32_t height) 
{
    m_Specification.Width = width;
    m_Specification.Height = height;

	RecalculateFramebuffer();
}

void OpenGLFramebuffer::RecalculateFramebuffer()
{
    // Delete existing if present
    if (m_RendererID)
    {
        glDeleteFramebuffers(1, &m_RendererID);
        glDeleteTextures(1, &m_ColorAttachment);
        glDeleteRenderbuffers(1, &m_DepthAttatchment);
    }

    // Create framebuffer
    glGenFramebuffers(1, &m_RendererID);
    glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

    // Create color texture/renderbuffer
    if (m_Specification.Samples > 1)
    {
        // Multisampled color buffer
        glGenTextures(1, &m_ColorAttachment);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_ColorAttachment);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_Specification.Samples, GL_RGBA8,
            m_Specification.Width, m_Specification.Height, GL_TRUE);

        // Attach multisampled color buffer
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_2D_MULTISAMPLE, m_ColorAttachment, 0);

        // Create and attach multisampled depth-stencil buffer
        glGenRenderbuffers(1, &m_DepthAttatchment);
        glBindRenderbuffer(GL_RENDERBUFFER, m_DepthAttatchment);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, m_Specification.Samples,
            GL_DEPTH24_STENCIL8, m_Specification.Width, m_Specification.Height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
            GL_RENDERBUFFER, m_DepthAttatchment);
    }
    else
    {
        // Regular (non-multisampled) color texture
        glGenTextures(1, &m_ColorAttachment);
        glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);

        GLenum minFilter = TextureFilterTypeToOpenGLType(m_Specification.MinFilter);
        GLenum maxFilter = TextureFilterTypeToOpenGLType(m_Specification.MaxFilter);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Specification.Width, m_Specification.Height,
            0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, maxFilter);

        // Add texture wrapping parameters for cleaner pixelation
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // Attach color texture
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
            m_ColorAttachment, 0);

        // Create and attach regular depth-stencil buffer
        glGenRenderbuffers(1, &m_DepthAttatchment);
        glBindRenderbuffer(GL_RENDERBUFFER, m_DepthAttatchment);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
            m_Specification.Width, m_Specification.Height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
            GL_RENDERBUFFER, m_DepthAttatchment);
    }

    // Check framebuffer status
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "Framebuffer Error: " << status << std::endl;
    }


    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}


