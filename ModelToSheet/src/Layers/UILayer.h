#pragma once
#include "Layer.h"
#include "Core/Log.h"

#include "Renderer/Model.h"	
#include "Events/LayerEvents.h"

class UILayer : public Layer {
public:
	UILayer()
		: 
		m_assetDirectory(std::filesystem::current_path() / "Models") {
		// Initialise the Presets
		m_Presets.push_back({ 90.0f, 270.0f, "Front" });
		m_Presets.push_back({ 90.0f, 90.0f, "Back" });
		m_Presets.push_back({ 90.0f, 0.0f, "Left" });
		m_Presets.push_back({ 90.0f, 180.0f, "Right" });
		m_Presets.push_back({ 0.0f, 270.0f, "Top" });
		m_Presets.push_back({ 180.0f, 270.0f, "Bottom" });

		// Creat the Modesl Directory if it doesn't exist
		if (!std::filesystem::exists(m_assetDirectory)) {
			std::filesystem::create_directories(m_assetDirectory);
		}
		m_CurrentDirectory = m_assetDirectory;
	}
	virtual  ~UILayer() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnEvent(Event& e) override;
	virtual void OnUpdate() override;
	virtual void OnImGuiRender() override;


private:
	void RenderLoggingTab();
	void RenderRepositoryTab();
	void RenderModelControls();
	void RenderAnimationControls();


	// Event functions
	bool OnModelLoadComplete(ModelLoadCompleteEvent& event);

private:
	struct ViewPreset {
		float PolarAngle;
		float AzimuthalAngle;
		std::string Name;
	};
	// Presets for buttons. Initialised in Constructor.
	std::vector<ViewPreset> m_Presets;

private:
	float m_CurrentPolarAngle = 90;
	float m_CurrentAzimuthalAngle = 270;


	// Assign directories
	const std::filesystem::path m_assetDirectory;
	std::filesystem::path m_CurrentDirectory;

	// Cached model
	std::shared_ptr<Model> m_Model = nullptr;
	std::shared_ptr<Animator> m_Animator = nullptr;
};