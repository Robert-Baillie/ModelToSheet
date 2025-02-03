#pragma once
#include "Texture.h"

#include <memory>

struct FramebufferSpecification {
	uint32_t Width = 0, Height = 0;
	uint32_t Samples = 1;
	TextureFilter MinFilter = TextureFilter::Linear;
	TextureFilter MaxFilter = TextureFilter::Linear;
};

class Framebuffer {
public:
	virtual ~Framebuffer() = default;

	virtual void Bind() const = 0;
	virtual void Unbind() const = 0;
	virtual void Resize(uint32_t width, uint32_t height) = 0;
	virtual uint32_t GetWidth() const = 0;
	virtual uint32_t GetHeight() const = 0;

	virtual uint32_t GetRendererID() const = 0;
	static std::shared_ptr<Framebuffer> Create(const FramebufferSpecification& spec);
	virtual uint32_t GetColorAttachmentRendererID() const = 0;
};