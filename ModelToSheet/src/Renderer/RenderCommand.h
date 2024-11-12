#pragma once
#include "RendererAPI.h"


// Render Command class serves as a static wrapper for the API which we can set the API type in.

// We do not need to rely on any virtual functions, we can always call this and the API will handle all logic.
class RenderCommand
{
public:
	
	inline static void SetClearColour(const glm::vec4& colour) { s_RendererAPI->SetClearColour(colour); }
	inline static void Clear() { s_RendererAPI->Clear(); }

	inline static void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) { s_RendererAPI->DrawIndexed(vertexArray); }

private:
	static RendererAPI* s_RendererAPI;

};