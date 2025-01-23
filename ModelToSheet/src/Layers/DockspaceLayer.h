#pragma once
#include "Layer.h"
#include <filesystem>

#include "Renderer/Framebuffer.h"

// A Dockspace layer to initialise the creation in window. Has to be the first layer
class DockspaceLayer : public Layer {
public:
    DockspaceLayer();
    virtual ~DockspaceLayer() = default;

    virtual void OnImGuiRender() override;
    virtual void OnAttach() override;
    virtual void OnDetach() override;
    void Begin();
    void End();

};