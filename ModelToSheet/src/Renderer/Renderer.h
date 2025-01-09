#pragma once
#include "RenderCommand.h"

#include "Scene/Camera.h"
#include "Shader.h"

#include "Scene/SceneData.h"

// Static Renderer Class that renders based on the SceneData and Camera given.
class Renderer
{
public:
	static void BeginScene(Camera& camera);
	static void EndScene();

	static void OnWindowResize(uint32_t width, uint32_t height);


	static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray);

	inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

private:
	static SceneData* m_SceneData;
};