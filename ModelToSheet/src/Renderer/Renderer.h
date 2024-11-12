#pragma once
#include "RenderCommand.h"

#include "Camera.h"
#include "Shader.h"

#include "SceneData.h"

// Static Renderer Class that renders based on the SceneData and Camera given.
class Renderer
{
public:
	static void BeginScene(Camera& camera);
	static void EndScene();

	static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray);

	inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

private:
	static SceneData* m_SceneData;
};