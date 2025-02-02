#include "pch.h"
#include "ViewportLayer.h"
#include "Core/Application.h"
#include "Core/Input.h"

#include <string>

#include "imgui.h"
#include "glm/gtc/matrix_transform.hpp"


#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

ViewportLayer::ViewportLayer() : Layer("ViewportLayer")
{
	// Test: Get a default model.
	std::string executablePath = std::filesystem::current_path().string();
	std::string modelPath = executablePath + "/Assets/Models/Test.fbx";
	m_Model = *RESOURCE_MANAGER.LoadModel(modelPath, "Weird Creature");

	// Load the Model Animation and Animator.
	m_Animation = new Animation(modelPath, &m_Model);
	m_Animator = new Animator(m_Animation);

	glm::vec3 modelCenter = m_Model.GetCenter();
	glm::vec3 dimensions = m_Model.GetDimensions();

	// Rotate the model so it is the correct way up and look at the camera
	m_ModelTransform = glm::mat4(1.0f);
	m_ModelTransform = glm::translate(glm::mat4(1.0f), modelCenter + glm::vec3(0.0f, dimensions.y / 2.0f, 0.0f));
	m_ModelTransform = glm::rotate(m_ModelTransform, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); 
	m_ModelTransform = glm::rotate(m_ModelTransform, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));

	// Assign Camera starting position
	float maxDimension = std::max({ dimensions.x, dimensions.y, dimensions.z });
	m_Camera = std::make_shared<OrthographicCamera>(-maxDimension, maxDimension, -maxDimension, maxDimension);
	m_Camera->SetPosition(modelCenter);

	// Assign Camera Orbit Settings
	m_OrbitCenter = modelCenter;
	m_OrbitRadius = maxDimension * 2.0f; // Initial Distance as above
	m_OrbitAzimuthal = glm::radians(180.0f);
	m_OrbitPolar = glm::radians(90.0f);

	RecalculateCameraPositionFromSphericalCoords();
}

void ViewportLayer::OnAttach()
{
	// Create the framebuffer with the size of the desired texture width
	m_Framebuffer = Framebuffer::Create(m_TextureWidth, m_TextureHeight);


	// Grab the shaders from Resource Manager. Initialise Shader Types
	m_FragmentShaders[FragmentShaderType::Diffuse] = RESOURCE_MANAGER.GetShader("DiffuseShader"); 
	m_FragmentShaders[FragmentShaderType::Normal] = RESOURCE_MANAGER.GetShader("NormalShader");
	m_CurrentShader = m_FragmentShaders[FragmentShaderType::Diffuse];
	m_CurrentFragmentShaderType = FragmentShaderType::Diffuse;


}

void ViewportLayer::OnDetach()
{
}

void ViewportLayer::OnUpdate()
{

	// Render OrthograpicView
	m_Framebuffer->Bind();
	RenderScene();
	m_Framebuffer->Unbind();


}

void ViewportLayer::OnImGuiRender()
{
	// Push the style. This needs to be popped eventuallly
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImVec2 viewportSize = ImGui::GetMainViewport()->Size;

	// Center the window

	ImGuiWindowFlags viewport_flags =
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoNav |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoScrollWithMouse;

	// Calculations for splitting viewports
	float twoThirdsWidth = viewportSize.x * (2.0f / 3.0f);
	float twoThirdsHeight = viewportSize.y * (2.0f / 3.0f);
	float oneThirdWidth = viewportSize.x * (1.0f / 3.0f);
	float oneThirdsHeight = viewportSize.y * (1.0f / 3.0f);


	// Render the perspective viewport - set in the top left
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(ImVec2(twoThirdsWidth, twoThirdsHeight));
	ImGui::Begin("Orthographic View", nullptr, viewport_flags);
	
	// Assigning the Framebuffer and windowsizes
	ImVec2 sizeIm = ImGui::GetContentRegionAvail();
	glm::vec2 newSize = { sizeIm.x, sizeIm.y };


	// If the size is not the same as the one we have loaded, then adjust the camera and framebuffer
	if (m_ViewSize != newSize && sizeIm.x > 0 && sizeIm.y > 0) {
		// Create a smaller framebuffer for pixelation effect. This will then be stretched to fill the viewport.
		m_Framebuffer->Resize(newSize.x / m_PixelScale, newSize.y / m_PixelScale);
		m_ViewSize = newSize;
	}

	int texID = m_Framebuffer->GetColorAttachmentRendererID();
	ImGui::Image((ImTextureID)(intptr_t)texID, sizeIm, ImVec2(0, 0), ImVec2(1, 1));


	// Overlaying the buttons. Get the upper and lower limits of the box
	ImVec2 start = ImGui::GetItemRectMin(); // Top Left
	ImVec2 end = ImGui::GetItemRectMax(); // Bottom right
	float buttonY = start.y + 10; // 10px from the top left
	float buttonX = start.x + 10;

	// Position the buttons at the top of the window
	ImGui::SetCursorScreenPos(ImVec2(buttonX, buttonY));
	if (ImGui::Button("Diffuse")) {
		m_CurrentShader =	m_FragmentShaders[FragmentShaderType::Diffuse];
		m_CurrentFragmentShaderType = FragmentShaderType::Diffuse;
	}

	ImGui::SameLine();
	if (ImGui::Button("Normal")) {
		m_CurrentShader = m_FragmentShaders[FragmentShaderType::Normal];
		m_CurrentFragmentShaderType = FragmentShaderType::Normal;

	}
	buttonY = end.y - 50; 
	buttonX = end.x - 75;

	ImGui::SetCursorScreenPos(ImVec2(buttonX, buttonY));
	if (ImGui::Button("Capture")) {
		CaptureScreenshot();
	}

	ImGui::End();

	ImGui::PopStyleVar();


	
}


void ViewportLayer::RenderScene(bool isCapturingScreenshot)
{
	// Clear the screen. No Transparency for Screenshot mode
	if(isCapturingScreenshot) RenderCommand::SetClearColour({ 0.0f, 0.0f, 0.0f, 0.0f });
	else RenderCommand::SetClearColour({ 0.8f, 0.8f, 0.8f, 1.0f });

	RenderCommand::Clear();

	// Begin scene
	Renderer::BeginScene(); // Currently calls nothing


	/// Bind the Shader and upload any uniforms
	m_CurrentShader->Bind();
	m_CurrentShader->UploadUniformMat4("u_ViewProjection", m_Camera->GetViewProjectionMatrix());
	// shader->UploadUniformFloat3("lightPos", camera->GetPosition()); These are not needed now we only use a 2D Viewport/
	// shader->UploadUniformFloat3("viewPos", camera->GetPosition());
	m_CurrentShader->UploadUniformMat4("u_Transform", m_ModelTransform);


	// Update for any Animation.
	if (m_Animator) {
		m_Animator->UpdateAnimation(0.016f);
		auto transform = m_Animator->GetFinalBoneMatrices();
		
		// Draw Bone Transforms
		m_CurrentShader->UploadUniformMat4Array("u_BoneTransforms", transform);

		// Draw based on Camera.
		m_Model.Draw(m_CurrentFragmentShaderType, m_CurrentShader);
	}

	Renderer::EndScene();
}

void ViewportLayer::RecalculateCameraPositionFromSphericalCoords()
{
	// Standard Spherical to Cartesian
	float x = m_OrbitRadius * sin(m_OrbitPolar) * cos(m_OrbitAzimuthal);
	float y = m_OrbitRadius * cos(m_OrbitPolar);
	float z = m_OrbitRadius * sin(m_OrbitPolar) * sin(m_OrbitAzimuthal);

	// Setting Position and what to look at.
	glm::vec3 position = m_OrbitCenter + glm::vec3(x, y, z);
	m_Camera->SetPosition(position);
	m_Camera->LookAt(m_OrbitCenter); // Ensure camera faces center
}


void ViewportLayer::CaptureScreenshot()
{
	// We want to loop through the shader types. Bind their respective shaders to the framebuffer, and Save the Framebuffer Texture

	// Sotre the original to restore later
	FragmentShaderType originalType = m_CurrentFragmentShaderType;

	// Loop through the shader types to create
	std::vector<FragmentShaderType>	 shaderTypes = { FragmentShaderType::Normal, FragmentShaderType::Diffuse };

	for (auto shaderType : shaderTypes) {
		// Set the current shader
		m_CurrentShader = m_FragmentShaders[shaderType];

		// Render the scene with this shader as usual
		m_Framebuffer->Bind();
		RenderScene(true);
		m_Framebuffer->Unbind();


		// Capture the framebuffer texture
		//SaveFramebufferTexture(m_Framebuffer, fmt::format("{}_map.png", GetShaderTypeName(shaderType)));


	}

	// Restore original shader
	m_CurrentShader = m_FragmentShaders[originalType];
}




