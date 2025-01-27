#include "pch.h"
#include "ViewportLayer.h"
#include "Core/Application.h"
#include "Core/Input.h"

#include <string>

#include "imgui.h"
#include "glm/gtc/matrix_transform.hpp"

ViewportLayer::ViewportLayer() : Layer("ViewportLayer")
{
	std::string executablePath = std::filesystem::current_path().string();
	std::string modelPath = executablePath + "/Assets/Models/Test.gltf";
	m_Model = *RESOURCE_MANAGER.LoadModel(modelPath, "Archer");
	m_Animation = new Animation(modelPath, &m_Model);
	m_Animator = new Animator(m_Animation);


	// Set model transform to rotate model if needed
	m_ModelTransform = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	glm::vec3 startingPos = m_ModelTransform[3];
	m_PerspectiveCamera = std::make_shared<PerspectiveCamera>(45.0f, 1.778f, 0.1f, 1000.0f);
	m_OrthographicCamera = std::make_shared<OrthographicCamera>(-10.0f, 10.0f, -10.0f, 10.0f);

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
	std::filesystem::path fragmentPath = currentPath.parent_path().parent_path() / "resources" / "Shaders" / "Fragment2D.shader";
	std::filesystem::path vertexPath = currentPath.parent_path().parent_path() / "resources" / "Shaders" / "Vertex2D.shader";
	m_OrthographicShader = RESOURCE_MANAGER.LoadShader(vertexPath.string(), fragmentPath.string(), "OrthographicShader");
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
	RenderScene(m_OrthographicCamera, m_OrthographicShader);
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
	float twoThirdsWidth = viewportSize.x * (2.0f/3.0f);
	float twoThirdsHeight = viewportSize.y * (2.0f / 3.0f);
	float oneThirdWidth = viewportSize.x * (1.0f / 3.0f);
	float oneThirdsHeight = viewportSize.y * (1.0f/3.0f);


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


	// Orthographic - basically as above - bottom right
	ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x + twoThirdsWidth, viewport->WorkPos.y + twoThirdsHeight));
	ImGui::SetNextWindowSize(ImVec2(oneThirdWidth, oneThirdsHeight));
	ImGui::Begin("2D View (Orthographic)", nullptr, viewport_flags);
	sizeIm = ImGui::GetContentRegionAvail(); 
	newSize = { sizeIm.x, sizeIm.y };
	// If the size is not the same as the one we have loaded, then adjust the camera and framebuffer
	if (m_OrthographicViewSize != newSize && sizeIm.x > 0 && sizeIm.y > 0) {
		m_OrthographicFramebuffer->Resize(newSize.x, newSize.y);
		m_OrthographicViewSize = newSize;
	}

	texID = m_OrthographicFramebuffer->GetColorAttachmentRendererID();
	ImGui::Image((ImTextureID)(intptr_t)texID, sizeIm, ImVec2(0, 0), ImVec2(1, 1));
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


	// Update the shader uniforms
	shader->Bind();
	shader->UploadUniformMat4("u_ViewProjection", camera->GetViewProjectionMatrix());
	shader->UploadUniformFloat3("lightPos", camera->GetPosition()); // Default for now;
	shader->UploadUniformFloat3("viewPos", camera->GetPosition());
	shader->UploadUniformMat4("u_Transform", m_ModelTransform);

	if (m_Animator) {
		m_Animator->UpdateAnimation(0.016f);
		auto transform = m_Animator->GetFinalBoneMatrices();
	

		shader->UploadUniformMat4Array("u_BoneTransforms", transform);

		m_Model.Draw();
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