#include "pch.h"
#include "OpenGLRendererAPI.h"

#include <glad/glad.h>

	
void OpenGLRendererAPI::SetClearColour(const glm::vec4& colour)
{
	glClearColor(colour.r, colour.g, colour.b, colour.a);
}

void OpenGLRendererAPI::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLRendererAPI::DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray)
{
	glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBufer()->GetCount(), GL_UNSIGNED_INT, nullptr);
}