#pragma once
#include "Layer.h"
#include "Core/Log.h"
#include "ViewportLayer.h"

class UILayer : public Layer {
public:
	UILayer(ViewportLayer* viewportLayer) : m_ViewportLayer(viewportLayer) {
		m_Presets.push_back({ 90.0f, 180.0f, "Front" });
		m_Presets.push_back({ 90.0f, 0.0f, "Back" });
		m_Presets.push_back({ 90.0f, 270.0f, "Left" });
		m_Presets.push_back({ 90.0f, 90.0f, "Right" });
		m_Presets.push_back({ 180.0f, 180.0f, "Top" });
		m_Presets.push_back({ 0.0f, 180.0f, "Bottom" });
	}
	virtual  ~UILayer() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnUpdate() override;
	virtual void OnImGuiRender() override;


private:
	void RenderLoggingTab();

private:
	struct ViewPreset {
		float PolarAngle;
		float AzimuthalAngle;
		std::string Name;
	};
	// Presets for buttons. Initialised in Constructor.
	std::vector<ViewPreset> m_Presets;

private:
	ViewportLayer* m_ViewportLayer = nullptr;
};