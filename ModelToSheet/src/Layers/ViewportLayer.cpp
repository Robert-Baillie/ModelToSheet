﻿#include "pch.h"
#include "ViewportLayer.h"
#include "Core/Application.h"
#include "Core/Input.h"

#include <string>

#include "imgui.h"
#include "glm/gtc/matrix_transform.hpp"

#include <glad/glad.h>// TEST TO REMOVE

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

ViewportLayer::ViewportLayer() : Layer("ViewportLayer")
{
	std::string executablePath = std::filesystem::current_path().string();
	std::string modelPath = executablePath + "/Assets/Models/Test.fbx";
	m_Model = *RESOURCE_MANAGER.LoadModel(modelPath, "Weird Creacher");
	m_Animation = new Animation(modelPath, &m_Model);
	m_Animator = new Animator(m_Animation);


	m_ModelTransform = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));

	glm::vec3 startingPos = m_ModelTransform[3];
	m_PerspectiveCamera = std::make_shared<PerspectiveCamera>(45.0f, 1.778f, 0.1f, 1000.0f);
	m_OrthographicCamera = std::make_shared<OrthographicCamera>(-1.0f, 1.0f, -1.0f, 1.0f);

	m_PerspectiveCamera->SetPosition(startingPos);
	m_OrthographicCamera->SetPosition(startingPos);
}

void ViewportLayer::OnAttach()
{
	// This should change to getting the window pointer from context and asigining
	Application& app = Application::Get();
	uint32_t width = app.GetWindow().GetWidth();
	uint32_t height = app.GetWindow().GetHeight();

	// Create the frame buffers
	m_PerspectiveFramebuffer = Framebuffer::Create(width, height);
	m_OrthographicFramebuffer = Framebuffer::Create(width, height);

	// Set the shaders
	std::filesystem::path currentPath = std::filesystem::path(__FILE__).parent_path();
	std::filesystem::path vertexPath = currentPath.parent_path().parent_path() / "resources" / "Shaders" / "Vertex.shader";
	std::filesystem::path diffusePath = currentPath.parent_path().parent_path() / "resources" / "Shaders" / "DiffuseF.shader";
	std::filesystem::path normalPath = currentPath.parent_path().parent_path() / "resources" / "Shaders" / "NormalF.shader";
	std::filesystem::path specularPath = currentPath.parent_path().parent_path() / "resources" / "Shaders" / "SpecularF.shader";
	m_FragmentShaders[FragmentShaderType::Diffuse] = RESOURCE_MANAGER.LoadShader(vertexPath.string(), diffusePath.string(), "DiffuseShader");
	m_FragmentShaders[FragmentShaderType::Normal] = RESOURCE_MANAGER.LoadShader(vertexPath.string(), normalPath.string(), "NormalShader");
	m_FragmentShaders[FragmentShaderType::Specular] = RESOURCE_MANAGER.LoadShader(vertexPath.string(), specularPath.string(), "SpecularShader");

	m_CurrentOrthographicShader = m_FragmentShaders[FragmentShaderType::Diffuse];
	m_PerspectiveShader = RESOURCE_MANAGER.GetDefaultShader();


}

void ViewportLayer::OnDetach()
{
}

void ViewportLayer::OnUpdate()
{

	// Get the camera inputs
	ControlCamera();


	/// Render Perspective View and bind to the framebuffer
	m_PerspectiveFramebuffer->Bind();
	RenderScene(m_PerspectiveCamera, m_PerspectiveShader);
	m_PerspectiveFramebuffer->Unbind();


	// Render OrthograpicView
	m_OrthographicFramebuffer->Bind();
	RenderScene(m_OrthographicCamera, m_CurrentOrthographicShader);
	m_OrthographicFramebuffer->Unbind();


}

void ViewportLayer::OnImGuiRender()
{
	const ImGuiViewport* viewport = ImGui::GetMainViewport();

	// Currently a test- creating horizontal layouts for both viewports
	// Push the style. This needs to be popped eventuallly
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));


	ImGuiWindowFlags viewport_flags =
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoNav |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoScrollWithMouse;

	// Calculations for splitting viewports
	ImVec2 viewportSize = ImGui::GetMainViewport()->Size;
	float twoThirdsWidth = viewportSize.x * (2.0f / 3.0f);
	float twoThirdsHeight = viewportSize.y * (2.0f / 3.0f);
	float oneThirdWidth = viewportSize.x * (1.0f / 3.0f);
	float oneThirdsHeight = viewportSize.y * (1.0f / 3.0f);


	// Render the perspective viewport - set in the top left
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(ImVec2(twoThirdsWidth, twoThirdsHeight));
	ImGui::Begin("3D View (Perspective)", nullptr, viewport_flags);
	ImVec2 sizeIm = ImGui::GetContentRegionAvail();
	glm::vec2 newSize = { sizeIm.x, sizeIm.y };

	// Adjust size if needed.
	if (m_PersepectiveViewSize != newSize && sizeIm.x > 0 && sizeIm.y > 0) {
		m_PerspectiveFramebuffer->Resize(sizeIm.x, sizeIm.y);
		m_PersepectiveViewSize = newSize;
		m_PerspectiveCamera->SetAspectRatio(sizeIm.x / sizeIm.y);
	}

	// Render
	uint32_t texID = m_PerspectiveFramebuffer->GetColorAttachmentRendererID();
	ImGui::Image((ImTextureID)(intptr_t)texID, sizeIm, ImVec2(0, 0), ImVec2(1, 1));
	ImGui::End();


	// Orthographic - position as above - bottom right
	ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x + twoThirdsWidth, viewport->WorkPos.y + twoThirdsHeight));
	ImGui::SetNextWindowSize(ImVec2(oneThirdWidth, oneThirdsHeight));
	ImGui::Begin("2D View (Orthographic)", nullptr, viewport_flags);


	// Assigning the Framebuffer and windowsizes
	sizeIm = ImGui::GetContentRegionAvail();
	newSize = { sizeIm.x, sizeIm.y };


	// If the size is not the same as the one we have loaded, then adjust the camera and framebuffer
	if (m_OrthographicViewSize != newSize && sizeIm.x > 0 && sizeIm.y > 0) {
		m_OrthographicFramebuffer->Resize(newSize.x, newSize.y);
		m_OrthographicViewSize = newSize;
	}
	texID = m_OrthographicFramebuffer->GetColorAttachmentRendererID();
	ImGui::Image((ImTextureID)(intptr_t)texID, sizeIm, ImVec2(0, 0), ImVec2(1, 1));


	// Overlaying the buttons. Get the upper and lower limits of the box
	ImVec2 start = ImGui::GetItemRectMin(); // Top Left
	ImVec2 end = ImGui::GetItemRectMax(); // Bottom right
	float buttonY = start.y + 10; // 10px from the top left
	float buttonX = start.x + 10;

	// Position the buttons at the top of the window
	ImGui::SetCursorScreenPos(ImVec2(buttonX, buttonY));
	if (ImGui::Button("Diffuse")) {
		m_CurrentOrthographicShader =	m_FragmentShaders[FragmentShaderType::Diffuse];
		m_CurrentFragmentShaderType = FragmentShaderType::Diffuse;
	}

	ImGui::SameLine();
	if (ImGui::Button("Normal")) {
		m_CurrentOrthographicShader = m_FragmentShaders[FragmentShaderType::Normal];
		m_CurrentFragmentShaderType = FragmentShaderType::Normal;

	}

	ImGui::SameLine();
	if (ImGui::Button("Specular")) {
		m_CurrentOrthographicShader = m_FragmentShaders[FragmentShaderType::Specular];
		m_CurrentFragmentShaderType = FragmentShaderType::Specular;

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

void ViewportLayer::RenderScene(std::shared_ptr<Camera> camera, std::shared_ptr<Shader> shader)
{
	
	// Clear the screen
	RenderCommand::SetClearColour({ 0.8f, 0.8f, 0.8f, 1.0f });
	RenderCommand::Clear();

	// Begin scene
	Renderer::BeginScene(); // Currently calls nothing


	// Update the shader uniforms based on the current fragment shader and camera type.
	shader->Bind();

	// ViewProjection active on Vertex for all
	shader->UploadUniformMat4("u_ViewProjection", camera->GetViewProjectionMatrix()); 
	
	// Light Position only in Fragment on Perspective
	if(camera->GetCameraType() == Camera::Type::Perspective) shader->UploadUniformFloat3("lightPos", camera->GetPosition()); // Default for now;

	// View Pos currently only in Fragment on Perspective
	if (camera->GetCameraType() == Camera::Type::Perspective)shader->UploadUniformFloat3("viewPos", camera->GetPosition());

	// Transform in Vertex for all.
	shader->UploadUniformMat4("u_Transform", m_ModelTransform);


	
	

	if (m_Animator) {
		m_Animator->UpdateAnimation(0.016f);
		auto transform = m_Animator->GetFinalBoneMatrices();
	
		// Bone Transform in vertex for all.
		shader->UploadUniformMat4Array("u_BoneTransforms", transform);

		// Draw based on Camera.
		if (camera->GetCameraType() == Camera::Type::Perspective) m_Model.Draw(FragmentShaderType::All, shader);
		else {
			// Pixel Size in the Orthographic Fragment Shader
			if(m_CurrentFragmentShaderType == FragmentShaderType::Diffuse) shader->UploadUniformFloat("u_PixelSize",0.0125f); // Pixel size of 0.0125 looks okay Create a slider to adjust this.
			m_Model.Draw(m_CurrentFragmentShaderType, shader);
		}
	}

	

	Renderer::EndScene();

}

void ViewportLayer::ControlCamera()
{
    float mouseX = Input::GetMouseX();
    float mouseY = Input::GetMouseY();
    
    if (isFirstMove)
    {
        lastMouseX = mouseX;
        lastMouseY = mouseY;
        isFirstMove = false;
    }
    
    // Fix inverted controls
    float xOffset = (mouseX - lastMouseX) * 0.5f; 
    float yOffset = (mouseY - lastMouseY) * 0.5f; 
    
    lastMouseX = mouseX;
    lastMouseY = mouseY;
    
    // Update camera rotation (yaw left/right, pitch up/down)
    m_PerspectiveCamera->SetYaw(m_PerspectiveCamera->GetYaw() + xOffset);
    m_PerspectiveCamera->SetPitch(m_PerspectiveCamera->GetPitch() + yOffset); 
    
    // Movement relative to camera orientation
    float moveSpeed = m_DeltaTime * m_CameraSpeed;
    if (Input::IsKeyPressed(KEY_W)) m_PerspectiveCamera->MoveForward(moveSpeed);
    if (Input::IsKeyPressed(KEY_S)) m_PerspectiveCamera->MoveBackward(moveSpeed);
    if (Input::IsKeyPressed(KEY_A)) m_PerspectiveCamera->MoveLeft(moveSpeed);
    if (Input::IsKeyPressed(KEY_D)) m_PerspectiveCamera->MoveRight(moveSpeed);
    if (Input::IsKeyPressed(KEY_Q)) m_PerspectiveCamera->MoveDown(moveSpeed);
    if (Input::IsKeyPressed(KEY_E)) m_PerspectiveCamera->MoveUp(moveSpeed);
    
    // Sync orthographic camera
    m_OrthographicCamera->SetPosition(m_PerspectiveCamera->GetPosition());
    m_OrthographicCamera->SetPitch(m_PerspectiveCamera->GetPitch());
    m_OrthographicCamera->SetYaw(m_PerspectiveCamera->GetYaw());
}

void ViewportLayer::CaptureScreenshot()
{
	// We want to loop through the shader types. Bind their respective shaders to the framebuffer, and Save the Framebuffer Texture

	// Sotre the original to restore later
	FragmentShaderType originalType = m_CurrentFragmentShaderType;

	// Loop through the shader types to create
	std::vector<FragmentShaderType>	 shaderTypes = { FragmentShaderType::Normal, FragmentShaderType::Diffuse, FragmentShaderType::Specular };

	for (auto shaderType : shaderTypes) {
		// Set the current shader
		m_CurrentOrthographicShader = m_FragmentShaders[shaderType];

		// Render the scene with this shader as usual
		m_OrthographicFramebuffer->Bind();
		RenderScene(m_OrthographicCamera, m_CurrentOrthographicShader);
		m_OrthographicFramebuffer->Unbind();


		// Capture the framebuffer texture
		SaveFramebufferTexture(m_OrthographicFramebuffer, fmt::format("{}_map.png", GetShaderTypeName(shaderType)));


	}

	// Restore original shader
	m_CurrentOrthographicShader = m_FragmentShaders[originalType];
}


void ViewportLayer::SaveFramebufferTexture(std::shared_ptr<Framebuffer> framebuffer, const std::string& filename)
{
	// Get the framebuffer dimensions
	int width = framebuffer->GetWidth();
	int height = framebuffer->GetHeight();

	// Allocate the data
	std::vector<unsigned char> pixels(width * height * 4);


	// Read pixels from framebuffer and assign to pixels => Needs to be abstracted out
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->GetRendererID());
	glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Flip image vertically (to match the opengl coordinate system)
	for (int line = 0; line != height / 2; ++line) {
		std::swap_ranges(
			pixels.begin() + 4 * width * line,
			pixels.begin() + 4 * width * (line + 1),
			pixels.begin() + 4 * width * (height - line - 1)
		);
	}


	// Save using stb => also needs abstracting.
	stbi_write_png(filename.c_str(), width, height, 4, pixels.data(), width * 4);
}



std::string ViewportLayer::GetShaderTypeName(FragmentShaderType type)
{
	switch (type)
	{
	case FragmentShaderType::Normal: return "normal";
	case FragmentShaderType::Specular: return "specular";
	case FragmentShaderType::Diffuse: return "diffuse";
	default: return "unknown";
	}
}