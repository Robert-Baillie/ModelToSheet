#include "pch.h"
#include "UILayer.h"

#include "Core/Application.h"
#include "Events/LayerEvents.h"

#include "imgui.h"



void UILayer::OnAttach()
{
}

void UILayer::OnDetach()
{
}

void UILayer::OnEvent(Event& e)
{
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<ModelLoadCompleteEvent>(BIND_EVENT_FN(UILayer::OnModelLoadComplete));
}

void UILayer::OnUpdate()
{
   
}

void UILayer::OnImGuiRender()
{
	// This is created after the main dockspace, so we just need to get the reference to it.
	const ImGuiViewport* viewport = ImGui::GetMainViewport();


	ImGuiWindowFlags viewport_flags =
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoNav |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoScrollWithMouse;

	// Calculations for splitting viewports
	ImVec2 viewportSize = viewport->Size;
	float twoThirdsWidth = viewportSize.x * (2.0f / 3.0f);
	float twoThirdsHeight = viewportSize.y * (2.0f / 3.0f);
	float oneThirdWidth = viewportSize.x * (1.0f / 3.0f);
	float oneThirdsHeight = viewportSize.y * (1.0f / 3.0f);


    /* Repository and Logging */
    ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x, viewport->WorkPos.y + twoThirdsHeight));
    ImGui::SetNextWindowSize(ImVec2(twoThirdsWidth, oneThirdsHeight));
    if (ImGui::Begin("Repository/Logging", nullptr, viewport_flags))
    {
        if (ImGui::BeginTabBar("Repository/Logging Bar"))
        {
            // Tab 1: Repository.
            RenderRepositoryTab();

            // Tab 2: Logging tab
            RenderLoggingTab();
            ImGui::EndTabBar();
        }

        ImGui::End();
    }


    /* Controls */
    ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x + twoThirdsWidth, viewport->WorkPos.y));
    ImGui::SetNextWindowSize(ImVec2(oneThirdWidth, viewportSize.y));
    if (ImGui::Begin("Controls Window", nullptr, viewport_flags))
    {
        // Below may need to be sectioned off
        RenderModelControls();

        RenderAnimationControls();
        RenderExportControls();
        
        ImGui::End();
    }
}

// Render what the logger receives.
void UILayer::RenderLoggingTab()
{
    if (ImGui::BeginTabItem("Log")) {

        // If the button to clear has been pressed, then clear the logger.
        if (ImGui::Button("Clear Logs")) {
            Log::Get().ClearLogs();
        }

        ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);// Create a scrollable area for the logs.

        // Display all logs, switch the colour formating based on the level of the log.
        for (const auto& log : Log::Get().GetLogs()) {
            switch (log.level) {
            case Log::Level::Error:
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f)); // Red
                break;
            case Log::Level::Warn:
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.5f, 0.0f, 1.0f)); // Yellow
                break;
            case Log::Level::Info:
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.1f, 0.75f, 0.0f, 1.0f)); // Green
                break;
            case Log::Level::Trace:
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f)); // Grey
                break;
            }
            ImGui::TextUnformatted(log.message.c_str());
            ImGui::PopStyleColor();
        }
        ImGui::EndChild();


        ImGui::EndTabItem();
    }
}

void UILayer::RenderRepositoryTab()
{
    if (ImGui::BeginTabItem("Repository")) {

        /* Control Bar*/
        // use all width (0) and 60 px height.
        ImGui::BeginChild("ControlBar", ImVec2(0, 50), true);
        
        // Left : Thumbnail control
        ImGui::SetNextItemWidth(200);
        static float thumbnailSize = 96;   // Can be altered below
        ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 64, 128);

        
        ImGui::SameLine(ImGui::GetWindowWidth() - 160);

        // Right: Button to open the folder
        if (ImGui::Button("Open Current Folder", ImVec2(150, 30))) {
            Helpers::OpenFolder(m_CurrentDirectory.string());
        }

        ImGui::EndChild();



        /* Repository Items */
        // Start the scrolling Region
        ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
        float padding = 16.0f;
        float cellSize = thumbnailSize + padding;
        float panelWidth = ImGui::GetContentRegionAvail().x;
        int columnCount = (int)(panelWidth / cellSize);
        if (columnCount < 1) columnCount = 1;

        // Set the amount of columns
        ImGui::Columns(columnCount, 0, false);

        // Back button if not in root directory
        if (m_CurrentDirectory != m_assetDirectory) {
            if (ImGui::Button("../", ImVec2(thumbnailSize, thumbnailSize))) {
                m_CurrentDirectory = m_CurrentDirectory.parent_path();
            }
            ImGui::NextColumn();
        }

        // First show folders
        for (auto& entry : std::filesystem::directory_iterator(m_CurrentDirectory)) {
            if (std::filesystem::is_directory(entry)) {
                std::string folderName = entry.path().filename().string();
                if (ImGui::Button((folderName + "/").c_str(), ImVec2(thumbnailSize, thumbnailSize))) {
                    m_CurrentDirectory /= folderName;
                }
                ImGui::NextColumn();

            }
        }

        // Models
        for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory)) {
            const auto& path = directoryEntry.path();
            std::string extension = path.extension().string();

            if (extension == ".obj" || extension == ".fbx" || extension == ".gltf" || extension == ".glb" || extension == ".3ds" || extension == ".dae") {
                std::string filename = path.filename().string();
                // Load the button
                if (ImGui::Button(filename.c_str(), ImVec2(thumbnailSize, thumbnailSize))) {
                    // If clicked, load this model by sending an event
                    std::string fullPath = m_CurrentDirectory.string() + "/" + filename;
                    ModelLoadStartEvent event(fullPath, filename);
                    Application::Get().OnEvent(event);

                }
                ImGui::NextColumn();
            }

        }
        ImGui::Columns(1);
        ImGui::EndChild();
        ImGui::EndTabItem();
    }
}

void UILayer::RenderModelControls()
{
    // Take 30% of the Right hand UI
    ImGui::SeparatorText("Camera Controls");
    ImGui::BeginChild("CameraControls", ImVec2(0, ImGui::GetWindowHeight() * 0.3f), true);

    // Begin Group for manual set controls
    ImGui::BeginGroup();
    float polarDegrees = m_CurrentPolarAngle; // Cached angle
    float azimuthalDegrees = m_CurrentAzimuthalAngle;

    // Sliders with formatting
    ImGui::Text("Orbit Controls");
    ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.9f);
    if (ImGui::SliderFloat("##Vertical", &polarDegrees, 0.0f, 180.0f, "Vertical: %.1f°"))
    {
       // The polar angle has been changed. Dispatch an event
        CameraOrbitEvent event(polarDegrees, azimuthalDegrees);
        Application::Get().OnEvent(event);
        
        m_CurrentPolarAngle = polarDegrees;
    }

    ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.9f);
    if (ImGui::SliderFloat("##Horizontal", &azimuthalDegrees, 0.0f, 360.0f, "Horizontal: %.1f°"))
    {
        CameraOrbitEvent event(polarDegrees, azimuthalDegrees);
        Application::Get().OnEvent(event);

        m_CurrentAzimuthalAngle = azimuthalDegrees;
    }
    ImGui::EndGroup();


    ImGui::Spacing();
    ImGui::Text("Orbit Controls");


    // Variables for Button Layout
    float windowWidth = ImGui::GetWindowWidth();
    float buttonWidth = 80;
    float buttonHeight = 30;
    int buttonsPerRow = (int)(windowWidth / (buttonWidth + 10));
    if (buttonsPerRow < 1) buttonsPerRow = 1;

    // Render each of the buttons
    for (int i = 0; i < m_Presets.size(); i++) {
        if (i % buttonsPerRow != 0) {
            ImGui::SameLine();
        }

        if (ImGui::Button(m_Presets[i].Name.c_str(), ImVec2(buttonWidth, buttonHeight))) {
            CameraOrbitEvent event(m_Presets[i].PolarAngle, m_Presets[i].AzimuthalAngle);
            Application::Get().OnEvent(event);
            m_CurrentPolarAngle = m_Presets[i].PolarAngle;
            m_CurrentAzimuthalAngle = m_Presets[i].AzimuthalAngle;
        }
    }

    ImGui::EndChild();

}

void UILayer::RenderAnimationControls()
{
    // Check if ther is a model
    if (!m_Model) {
        ImGui::Text("No model loaded.");
        return;
    }

    // Get the animations and model.
    ImGui::SeparatorText("Animations");

   
    const auto& animations = m_Model->GetAnimations();

    if (animations.empty()) {
        ImGui::Text("No animations available.");
        return;
    }

    /* Animation List */
    // Loop through the animations.
    if (ImGui::BeginChild("AnimationList", ImVec2(0, 120), true)) {

        for (const auto& [name, anim] : animations) {

            if (ImGui::Selectable(name.c_str(), m_Animator->GetCurrentAnimation()->GetName() == name)) {
                // This has been chosen, change this animation
                AnimationChangeEvent event(name);
                Application::Get().OnEvent(event);

                // Should load by now, find the new animation and resize the slider.
                m_KeyFrames.clear();
                m_KeyFrames.resize(m_Animator->GetCurrentAnimation()->GetKeyframeCount(), false);
                m_SliderValue = 0;
            }


            // Show animation details on hover
            if (ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                ImGui::Text("Duration: %.2f s", anim->GetDuration() / anim->GetTicksPerSecond());
                ImGui::Text("Frames: %d", anim->GetKeyframeCount());
                ImGui::EndTooltip();
            }
        }

        ImGui::EndChild();
    }


    ImGui::SeparatorText("Animation Controls");
    /* Frame Selection  Control */
    if (ImGui::BeginChild("Frame selection", ImVec2(0, 300), true)) {

        // Integer Slider to show the current KeyFrame
        ImGui::Text("KeyFrames: ");
        ImGui::NewLine(); // Space down a little
        if (ImGui::SliderInt("##KeyFrames", &m_SliderValue, 0, (m_KeyFrames.size() - 1))) {
            KeyframeChangeEvent event(m_SliderValue);
            Application::Get().OnEvent(event);

            // As with viewport player, this is now not playing in favour of showing the user the slider.
            m_IsAnimationPlaying = false;
        }
        ImVec2 p_min = ImGui::GetItemRectMin();
        ImVec2 p_max = ImGui::GetItemRectMax();
        // Display Orange circles for values which are checked
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        float slider_width = p_max.x - p_min.x;

        // Start by looping over the length of the keyframes
        for (int i = 0; i < m_KeyFrames.size(); i++) {
            // If the value is checked, then draw the circle
            if (m_KeyFrames[i]) {
                float pos_x = p_min.x + ((float)i / (m_KeyFrames.size() - 1)) * slider_width;
                draw_list->AddCircleFilled(ImVec2(pos_x, p_min.y - 10), 5.0f, IM_COL32(255, 165, 0, 255));  // Orange circle
            }
        }

        // Render a single check box. If the check box is check according to the current slider value then display it as clicked.
        bool temp = m_KeyFrames[m_SliderValue];
        if (ImGui::Checkbox("Include Frame: ", &temp))
        {
            // Has been clicked assign the bool.
            m_KeyFrames[m_SliderValue] = temp;

            // Add all indexes to list and pass to listeners
            std::vector<int> indexList;
            for (int i = 0; i < m_KeyFrames.size(); i++) {
                if (m_KeyFrames[i]) indexList.push_back(i);
            }
            AnimationKeyChangeEvent event(indexList);
            Application::Get().OnEvent(event);

        }
        ImGui::EndChild();
    }
}

void UILayer::RenderExportControls()
{
    // Add buttons at the bottom right
    ImGui::Spacing();
    ImGui::Separator();

    // Calculate button positions
    float buttonWidth = 120.0f;
    float buttonHeight = 40.0f;
    ImVec2 windowSize = ImGui::GetWindowSize();
    ImGui::SetCursorPos(ImVec2(windowSize.x - buttonWidth - 20.0f, windowSize.y - buttonHeight - 20.0f));   // Assign to the bottom right, adjust by width and padding

    bool canDisable = (m_Model != nullptr);
    if (!canDisable) ImGui::BeginDisabled(true);
    if (ImGui::Button("Export", ImVec2(buttonWidth, buttonHeight))) {
        ExportEvent event;
        Application::Get().OnEvent(event);
    }
    if (!canDisable) {
        ImGui::EndDisabled();
    }

    ImGui::SameLine();
    ImGui::SetCursorPosX(windowSize.x - (2 * buttonWidth) - 40.0f); // More Left than the above

    if (ImGui::Button("Open Screenshot", ImVec2(buttonWidth, buttonHeight))) {
        // Handle open screenshot action
        std::string path = std::filesystem::current_path().string() + "/Screenshots/";
        Helpers::OpenFolder(path);
    }
}

// Event functions

bool UILayer::OnModelLoadComplete(ModelLoadCompleteEvent& event)
{
    // Simply cache the model. Animator will have been loaded too.
    m_Model = event.GetModel();
    m_Animator = event.GetAnimator();
    m_KeyFrames.clear();
    m_KeyFrames.resize(m_Animator->GetCurrentAnimation()->GetKeyframeCount(), false);
    m_SliderValue = 0;
    // Nothing else needs this
    return true;
}
