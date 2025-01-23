#include "pch.h"
#include "UILayer.h"

#include "imgui.h"

UILayer::UILayer()
{
}

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
    if (ImGui::Begin("Tab Window", nullptr, viewport_flags))
    {
        if (ImGui::BeginTabBar("MyTabBar"))
        {
            // Tab 1: First component
            if (ImGui::BeginTabItem("Component 1"))
            {
                ImGui::Text("This is Component 1");
                ImGui::Button("Button 1");
                ImGui::EndTabItem();
            }

            // Tab 2: Second component
            if (ImGui::BeginTabItem("Component 2"))
            {
                ImGui::Text("This is Component 2");
                ImGui::Button("Button 2");
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        ImGui::End();
    }


    /* Controls */
    ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x + twoThirdsWidth, viewport->WorkPos.y));
    ImGui::SetNextWindowSize(ImVec2(oneThirdWidth, twoThirdsHeight));
    if (ImGui::Begin("Controls Window", nullptr, viewport_flags))
    {
        
        ImGui::Text("This is Component 1");
        ImGui::Button("Button 1");
          

        ImGui::End();
    }
}
