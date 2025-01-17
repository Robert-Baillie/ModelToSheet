#pragma once
#include "RenderCommand.h"

#include "Scene/Camera.h"
#include "Shader.h"


// Static Renderer Class that renders based on the SceneData and Camera given.
class Renderer
{
public:
	static void BeginScene();
	static void EndScene();

	static void OnWindowResize(uint32_t width, uint32_t height);


	static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, std::shared_ptr<Camera> camera);

	inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

};