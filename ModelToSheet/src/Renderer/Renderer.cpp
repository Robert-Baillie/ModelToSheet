#include "pch.h"
#include "Renderer.h"


void Renderer::BeginScene()
{
}

void Renderer::EndScene()
{

}

void Renderer::OnWindowResize(uint32_t width, uint32_t height)
{
	RenderCommand::SetViewport(0, 0, width, height);
}

void Renderer::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray,
					std::shared_ptr<Camera> camera)
{
	shader->Bind();
	shader->UploadUniformMat4("u_ViewProjection", camera->GetViewProjectionMatrix());
	
	// Submit into the render command queue
	vertexArray->Bind();
	RenderCommand::DrawIndexed(vertexArray);
}
