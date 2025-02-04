#include "pch.h"
#include "UILayer.h"

#include "imgui.h"


void UILayer::OnAttach()
{
}

void UILayer::OnDetach()
{
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
        float polarDegrees = m_ViewportLayer->GetPolarAngle();
        float azimuthalDegrees = m_ViewportLayer->GetAzimuthalAngle();


        if (ImGui::SliderFloat("Vertical Angle", &polarDegrees, 0.0f, 180.0f))
        {
            m_ViewportLayer->SetPolarAngle(polarDegrees);
            m_ViewportLayer->RecalculateCameraPositionFromSphericalCoords();
        }

        if (ImGui::SliderFloat("Horizontal Angle", &azimuthalDegrees, 0.0f, 360.0f))
        {
            m_ViewportLayer->SetAzimuthalAngle(azimuthalDegrees);
            m_ViewportLayer->RecalculateCameraPositionFromSphericalCoords();
        }

        // Render each of the buttons
        for (int i = 0; i < m_Presets.size(); i++) {
           if(ImGui::Button(m_Presets[i].Name.c_str(), ImVec2(60, 25))) {
               // Button hit, set the angles
               m_ViewportLayer->SetPolarAngle(m_Presets[i].PolarAngle);
               m_ViewportLayer->SetAzimuthalAngle(m_Presets[i].AzimuthalAngle);
               m_ViewportLayer->RecalculateCameraPositionFromSphericalCoords();

            }
        }


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

        
        // Get the columns that can fit into the repository
        static float thumbnailSize = 96;   // Can be altered below
        ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 64, 128);



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
                    // If clicked, load this model
                    std::string fullPath = m_CurrentDirectory.string() + "/" + filename;
                    m_ViewportLayer->LoadModel(fullPath, filename);
                }
                ImGui::NextColumn();
            }

        }
        ImGui::Columns(1);
        ImGui::EndChild();
        ImGui::EndTabItem();
    }
}
