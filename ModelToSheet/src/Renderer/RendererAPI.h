#pragma once
#include <glm/glm.hpp>

#include "VertexArray.h"

// Static Renderer API. Sets the renderer type. Should be adopted by whatever renderer we use.
class RendererAPI
{
public:
	enum class API
	{
		None = 0, OpenGL = 1
	};

public:
	virtual void SetClearColour(const glm::vec4& colour) = 0;
	virtual void Clear() = 0;

	virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;

	virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) = 0;
	virtual void ReadPixels(uint32_t width, uint32_t height, std::vector<unsigned char>& pixels) = 0;

	inline static API GetAPI() { return s_API; }

private:
	static API s_API;
};