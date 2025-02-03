#pragma once

#include "Renderer/RendererAPI.h"

class OpenGLRendererAPI : public RendererAPI
{
	virtual void SetClearColour(const glm::vec4& colour) override;
	virtual void Clear() override;
	virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

	virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) override;
	virtual  void ReadPixels(uint32_t width, uint32_t height, std::vector<unsigned char>& pixels) override;

};