#include "pch.h"
#include "Renderer.h"

SceneData* Renderer::m_SceneData = new SceneData;

void Renderer::BeginScene(Camera& camera)
{
	m_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
}

void Renderer::EndScene()
{

}

void Renderer::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray)
{
	shader->Bind();
	shader->UploadUniformMat4("u_ViewProjection", m_SceneData->ViewProjectionMatrix);
	
	// Submit into the render command queue
	vertexArray->Bind();
	RenderCommand::DrawIndexed(vertexArray);
}
