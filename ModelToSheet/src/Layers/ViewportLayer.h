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

    // ImGui Methods
    void ShowExportWarning();

    // Event functions
    bool OnModelLoadStart(ModelLoadStartEvent& event);
    bool OnCameraOrbitChange(CameraOrbitEvent& event);
    bool OnAnimationChange(AnimationChangeEvent& event);
    bool OnExport(ExportEvent& event);
    bool OnKeyFrameChanged(AnimationKeyChangeEvent& event);
    bool OnKeyFrameSliderValueChanged(KeyframeChangeEvent& event);
    bool OnTextureSizeChange(TextureSizeChangeEvent& event);

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
    bool m_IsModelLoading = false;

    // Animation Settings
    std::shared_ptr<Animator> m_Animator = nullptr;
    Animation* m_CurrentAnimation = nullptr;
    std::vector<int> m_KeyFrameIndexList;

    // Times
    float m_FrameTime = 0.016f; // 60fps
    bool m_IsPlaying = true;

    // Preview
    int m_CurrentPreviewFrame = 0;
    float m_AccumulatedPreviewTime = 0.0f;
    bool m_KeyframePreviewMode = false;
    int m_PreviewSamplesPerSecond = 10;

    // Flow control
    FragmentShaderType m_CurrentFragmentShaderType = FragmentShaderType::Diffuse;
    std::unordered_map<FragmentShaderType, std::shared_ptr<Shader>> m_FragmentShaders;

    // Control for Texture Size
    int m_TextureWidth = 128, m_TextureHeight = 128; // as it is in uilayer

    // Camera Orbit Controls7
    glm::vec3 m_OrbitCenter;
    float m_OrbitRadius; // r in Spherical Coords
    float m_OrbitAzimuthal; // Azimuthal angle (around Y cartesian axis)
    float m_OrbitPolar; // Polar angle (from Y axis)

    // imgui controls
    bool m_ShowExportWarning = false;

};