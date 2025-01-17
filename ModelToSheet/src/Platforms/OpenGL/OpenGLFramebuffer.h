#pragma once

#include "Renderer/FrameBuffer.h"

class OpenGLFramebuffer : public Framebuffer {
public:
	OpenGLFramebuffer(uint32_t width, uint32_t height);
	virtual ~OpenGLFramebuffer();

	virtual void Bind() const override;
	virtual void Unbind() const override;
	virtual void Resize(uint32_t width, uint32_t height)  override;
	virtual uint32_t GetColorAttachmentRendererID() const override { return m_ColorAttachment; }


private:
	void RecalculateFramebuffer();
private:
	uint32_t m_RendererID = 0;
	uint32_t m_ColorAttachment = 0;
	uint32_t m_DepthAttatchment = 0;
	uint32_t m_Width, m_Height;
};