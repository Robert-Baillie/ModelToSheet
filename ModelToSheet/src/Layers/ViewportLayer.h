#pragma once
#include "Layer.h"
#include <filesystem>

#include "Renderer/Framebuffer.h"

// A Layer which holds the framebuffer data for both the 3D scene and 2D scene.
class ViewportLayer : public Layer {
public:
    ViewportLayer();
    virtual ~ViewportLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnUpdate() override;
    virtual void OnImGuiRender() override;

private:
    void RenderScene(std::shared_ptr<Camera> camera, std::shared_ptr<Shader> shader);


private:
    // Cameras
    std::shared_ptr<PerspectiveCamera> m_PerspectiveCamera;
    std::shared_ptr<OrthographicCamera> m_OrthographicCamera;

    // Shaders
    std::shared_ptr<Shader> m_PerspectiveShader;
    std::shared_ptr<Shader> m_OrthographicShader;

    // Framebuffers for both cameras
    std::shared_ptr<Framebuffer> m_PerspectiveFramebuffer;
    std::shared_ptr<Framebuffer> m_OrthographicFramebuffer;

    // Sizes for viewports
    glm::vec2 m_PersepectiveViewSize = { 0.0f, 0.0f };
    glm::vec2 m_OrthographicViewSize = { 0.0f, 0.0f };

    // Model settings
    std::shared_ptr<Model> m_Model;
    glm::mat4 m_ModelTransform; 
    float m_ModelRotation = 0.0f;
    float m_RotationSpeed = 20.0f;
    float m_DeltaTime = 0.016f;
};