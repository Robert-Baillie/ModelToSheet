#pragma once
#include "Layer.h"
#include <filesystem>

#include "Renderer/Framebuffer.h"
#include "Renderer/Animation/Animator.h"

#include "Events/LayerEvents.h"

// A Layer which holds the framebuffer data for both the 3D scene and 2D scene.
class ViewportLayer : public Layer {
public:
    ViewportLayer();
    virtual ~ViewportLayer() {
        ClearCurrentModel();
    };

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnEvent(Event& event) override;
    virtual void OnUpdate() override;
    virtual void OnImGuiRender() override;
   
    
private:
    void RenderScene(bool isCapturingScreenshot = false);
    
    void ClearCurrentModel() { delete m_CurrentAnimation; }
    void LoadModel(const std::string& path, const std::string& name);

    void RecalculateCameraPositionFromSphericalCoords();

    // Export Functionality
    void ExportAnimationSpriteSheet();


    // Event functions
    bool OnModelLoadStart(ModelLoadStartEvent& event);
    bool OnCameraOrbitChange(CameraOrbitEvent& event);
    bool OnAnimationChange(AnimationChangeEvent& event);

private:
    // Camera
    std::shared_ptr<OrthographicCamera> m_Camera;

    // Shader
    std::shared_ptr<Shader> m_CurrentShader;

    // Framebuffer
    std::shared_ptr<Framebuffer> m_Framebuffer;

    // Sizes for viewports
    glm::vec2 m_ViewSize = { 0.0f, 0.0f };

    // Model settings
    std::shared_ptr<Model> m_Model = nullptr;
    glm::mat4 m_ModelTransform = glm::mat4(1.0f);

    // Animation Settings
    std::shared_ptr<Animator> m_Animator = nullptr;
    Animation* m_CurrentAnimation = nullptr;

    // Times
    float m_DeltaTime= 0.016f;
    float m_LastFrameTime = 0.0f;


    // Flow control
    FragmentShaderType m_CurrentFragmentShaderType = FragmentShaderType::Diffuse;
    std::unordered_map<FragmentShaderType, std::shared_ptr<Shader>> m_FragmentShaders;

    // Control for Texture Size
    uint32_t m_TextureWidth = 512, m_TextureHeight = 512;

    // Camera Orbit Controls7
    glm::vec3 m_OrbitCenter;
    float m_OrbitRadius; // r in Spherical Coords
    float m_OrbitAzimuthal; // Azimuthal angle (around Y cartesian axis)
    float m_OrbitPolar; // Polar angle (from Y axis)

};