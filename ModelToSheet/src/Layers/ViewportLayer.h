#pragma once
#include "Layer.h"
#include <filesystem>

#include "Renderer/Framebuffer.h"
#include "Renderer/Animation/Animator.h"

// A Layer which holds the framebuffer data for both the 3D scene and 2D scene.
class ViewportLayer : public Layer {
public:
    ViewportLayer();
    virtual ~ViewportLayer() {
        delete m_Animation;
        delete m_Animator;
    };

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnUpdate() override;
    virtual void OnImGuiRender() override;

private:
    void RenderScene(std::shared_ptr<Camera> camera, std::shared_ptr<Shader> shader);
    void ControlCamera();


private:
    // Cameras
    std::shared_ptr<PerspectiveCamera> m_PerspectiveCamera;
    std::shared_ptr<OrthographicCamera> m_OrthographicCamera;

    bool isFirstMove = true;
    float lastMouseX = 0.0f;
    float lastMouseY = 0.0f;

    float m_CameraSpeed = 5.0f;
    float m_CameraRotationSpeed = 1.0f;

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
    Model m_Model; // should be a ptr
    glm::mat4 m_ModelTransform = glm::mat4(1.0f);
    float m_ModelRotation = 0.0f;
    float m_RotationSpeed = 20.0f;

    Animator* m_Animator;
    Animation* m_Animation;

    // Times
    float m_DeltaTime= 0.016f;
    float m_LastFrameTime = 0.0f;

  
};