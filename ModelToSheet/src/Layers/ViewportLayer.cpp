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
	// Create the camera
	m_Camera = std::make_shared<OrthographicCamera>(-1.0f, 1.0f, -1.0f, 1.0f);

	m_OrbitPolar = glm::radians(90.0f);
	m_OrbitAzimuthal = glm::radians(270.0f);
}

void ViewportLayer::OnAttach()
{
	// Create the framebuffer with the size of the desired texture width
	FramebufferSpecification spec;
	spec.Width = m_TextureWidth;
	spec.Height = m_TextureHeight;
	spec.MinFilter = TextureFilter::Nearest;
	spec.MaxFilter = TextureFilter::Nearest;
	spec.Samples = 1;
	m_Framebuffer = Framebuffer::Create(spec);
	m_Framebuffer->Resize(m_TextureHeight , m_TextureHeight );

	// Grab the shaders from Resource Manager. Initialise Shader Types
	m_FragmentShaders[FragmentShaderType::Diffuse] = RESOURCE_MANAGER.GetShader("DiffuseShader"); 
	m_FragmentShaders[FragmentShaderType::Normal] = RESOURCE_MANAGER.GetShader("NormalShader");
	m_CurrentShader = m_FragmentShaders[FragmentShaderType::Diffuse];
	m_CurrentFragmentShaderType = FragmentShaderType::Diffuse;


}

void ViewportLayer::OnDetach()
{
}

void ViewportLayer::OnEvent(Event& e)
{
	 EventDispatcher dispatcher(e);
	 dispatcher.Dispatch<ModelLoadStartEvent>(BIND_EVENT_FN(ViewportLayer::OnModelLoadStart));
	 dispatcher.Dispatch<CameraOrbitEvent>(BIND_EVENT_FN(ViewportLayer::OnCameraOrbitChange));
	 dispatcher.Dispatch<AnimationChangeEvent>(BIND_EVENT_FN(ViewportLayer::OnAnimationChange));
	 dispatcher.Dispatch<ExportEvent>(BIND_EVENT_FN(ViewportLayer::OnExport));
	 dispatcher.Dispatch<AnimationFPSChangeEvent>(BIND_EVENT_FN(ViewportLayer::OnFPSChanged));
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

	/***** RENDERING THE PIXELATED MODEL *****/


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



	int texID = m_Framebuffer->GetColorAttachmentRendererID();
	ImVec2 textureSize(m_TextureWidth, m_TextureHeight);
	ImVec2 winPos = ImGui::GetCursorScreenPos();
	ImVec2 center = ImVec2(winPos.x + (sizeIm.x - textureSize.x) * 0.5f,
		winPos.y + (sizeIm.y - textureSize.y) * 0.5f);
	ImGui::SetCursorScreenPos(center);
	ImGui::Image((ImTextureID)(intptr_t)texID, textureSize, ImVec2(0, 1), ImVec2(1, 0)); // THis is what was causing the flipped model rotation at the start.

	/***** RENDERING THE BUTTONS *****/

	// Overlaying the buttons. Get the upper and lower limits of the box
	float paddingX = 10.0f;
	float paddingY = 25.0f;

	// Position the buttons at the top of the window
	ImGui::SetCursorScreenPos(ImVec2(viewport->WorkPos.x + paddingX, viewport->WorkPos.y + paddingY));
	if (ImGui::Button("Diffuse")) {
		m_CurrentShader =	m_FragmentShaders[FragmentShaderType::Diffuse];
		m_CurrentFragmentShaderType = FragmentShaderType::Diffuse;
	}

	ImGui::SameLine();
	if (ImGui::Button("Normal")) {
		m_CurrentShader = m_FragmentShaders[FragmentShaderType::Normal];
		m_CurrentFragmentShaderType = FragmentShaderType::Normal;

	}



	/* Pause/Play button */
	ImVec2 buttonSize(100, 30);
	ImGui::SetCursorScreenPos(ImVec2(
		ImGui::GetWindowPos().x + (sizeIm.x - 100) * 0.5f,
		ImGui::GetWindowPos().y + sizeIm.y - 40
	));
	if (ImGui::Button(m_IsPlaying ? "Pause" : "Play")) {
		m_IsPlaying = !m_IsPlaying;
	}
	ImGui::End();

	ImGui::PopStyleVar();


	
}



void ViewportLayer::LoadModel(const std::string& path, const std::string& name)
{
	// Clear current model
	ClearCurrentModel();

	// Try find model if it has been loaded before
	m_Model = RESOURCE_MANAGER.GetModel(name);

	// Else try to load it
	if (!m_Model) m_Model = RESOURCE_MANAGER.LoadModel(path, name);

	// If it loaded successful, recreate all nevessary variables and animations
	m_Animator = std::make_shared<Animator>();

	for (const auto& [name, anim] : m_Model->GetAnimations()) {
		m_Animator->AddAnimation(name, anim);	// This will play the first animation added.
	}

	glm::vec3 modelCenter = m_Model->GetCenter();
	glm::vec3 dimensions = m_Model->GetDimensions();

	float maxDimension = std::max({ dimensions.x, dimensions.y, dimensions.z });
	m_Camera->SetProjection(-maxDimension, maxDimension, -maxDimension, maxDimension, -100.0f, 100.0f);

	m_ModelTransform = glm::translate(glm::mat4(1.0f),
		modelCenter - glm::vec3(0.0f, dimensions.y / 2.0f, 0.0f));

	m_OrbitCenter = modelCenter;
	m_OrbitRadius = maxDimension * 2.0f;
	RecalculateCameraPositionFromSphericalCoords();
}


void ViewportLayer::RenderScene(bool isCapturingScreenshot)
{
	// Clear the screen. No Transparency for Screenshot mode
	RenderCommand::SetClearColour({ 0.0f, 0.0f, 0.0f, 0.0f });

	RenderCommand::Clear();

	// Begin scene
	Renderer::BeginScene(); // Currently calls nothing

	if (m_Model && m_CurrentShader) {
		/// Bind the Shader and upload any uniforms
		m_CurrentShader->Bind();
		m_CurrentShader->UploadUniformMat4("u_ViewProjection", m_Camera->GetViewProjectionMatrix());
		// shader->UploadUniformFloat3("lightPos", camera->GetPosition()); These are not needed now we only use a 2D Viewport/
		// shader->UploadUniformFloat3("viewPos", camera->GetPosition());
		m_CurrentShader->UploadUniformMat4("u_Transform", m_ModelTransform);


		// Update for any Animation.
		if (m_Animator  &&m_IsPlaying) {
			m_Animator->UpdateAnimation(m_FrameTime);
			auto transform = m_Animator->GetFinalBoneMatrices();

			// Draw Bone Transforms
			m_CurrentShader->UploadUniformMat4Array("u_BoneTransforms", transform);
		}

		// Draw based on Camera.
		m_Model->Draw(m_CurrentFragmentShaderType, m_CurrentShader);
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


void ViewportLayer::ExportAnimationSpriteSheet()
{
	// TEST FOR NOW
	int frameCount = 25; // How many we are capturing, this can be defined by the user

	float originalTime = m_Animator->GetCurrentTime(); 
	int originalFrame = m_Animator->GetCurrentFrame(); 

	// Store the original shader type to redisplay at the end. Get the shaderTypes we want to export into files.
	FragmentShaderType originalType = m_CurrentFragmentShaderType;
	std::vector<FragmentShaderType> shaderTypes = { FragmentShaderType::Diffuse, FragmentShaderType::Normal };

	// Looping over these types
	for (auto shaderType : shaderTypes) {
		// Assign the current shader based n this type.
		m_CurrentShader = m_FragmentShaders[shaderType];
		m_CurrentFragmentShaderType = shaderType;

		// We want to export the image, for this we need to have a set of pixel data that is large enough to store the width of each frame, by the height, but the size of a char (4) and by the amount of frames being captured.
		std::vector<unsigned char> spriteSheetPixels(m_TextureWidth * m_TextureHeight * 4 * frameCount);
		
		

		// Loop through the frames for this shader
		for (int frame = 0; frame < frameCount; frame++) {
			// Go to the next frame. We are simulating 60 fps below
			m_Animator->UpdateAnimation(m_FrameTime);

			// Redner the scene based on this frame
			m_Framebuffer->Bind();
			RenderScene();

			// Read the pixels of thisframe and temporarily store
			std::vector<unsigned char> framePixels(m_TextureWidth * m_TextureHeight * 4);
			RenderCommand::ReadPixels(m_TextureWidth, m_TextureHeight, framePixels);
			m_Framebuffer->Unbind();

			// Add this into the final spritesheet pixels
			for (int y = 0; y < m_TextureHeight; y++) {
				size_t destOffset =
					y * (m_TextureWidth * frameCount * 4) +  // Start of row y in the sprite sheet
					frame * (m_TextureWidth * 4);            // Horizontal position for this frame

				memcpy(
					spriteSheetPixels.data() + destOffset,	// Destination
					framePixels.data() + ((m_TextureHeight - 1 - y) * m_TextureWidth * 4),
					m_TextureWidth * 4
				);
			}

			
		}
		

		// Save the sprite sheet into destination screenshots. Making sure that screen shots exists
		std::string filename = fmt::format("{}_animation_spritesheet.png", Shader::GetShaderTypeName(shaderType));
		std::string executablePath = std::filesystem::current_path().string();
		std::string fullPath = executablePath + "/Screenshots/" + filename;
		std::filesystem::create_directories(std::filesystem::path(fullPath).parent_path());
		

		stbi_write_png(
			fullPath.c_str(),
			m_TextureWidth * frameCount,  // Total width is frames side by side
			m_TextureHeight,
			4,
			spriteSheetPixels.data(),
			m_TextureWidth * 4 * frameCount
		);

		// Restore the time for the next shader
		m_Animator->SetCurrentTime(originalTime);

		

	}

	// Restore the time for the users seemless experience
	m_Animator->SetCurrentTime(originalTime);

	// Restore original shader
	m_CurrentShader = m_FragmentShaders[originalType];
	m_CurrentFragmentShaderType = originalType;

}


/* Event Methods */
bool ViewportLayer::OnModelLoadStart(ModelLoadStartEvent& event)
{
	// A model has been clicked in the UI. Load it as intended
	LoadModel(event.GetPath(), event.GetModelName());


	// Annoyingly, we need to cache this model pointer back to UI layer, so call an event back
	ModelLoadCompleteEvent eventTwo(m_Model, m_Animator);
	Application::Get().OnEvent(eventTwo);

	// No other events need this, return true.
	return true;
}

bool ViewportLayer::OnCameraOrbitChange(CameraOrbitEvent& event)
{
	// We do not know which has changed, assign both.
	// This comes in as a degree value, we want it to be radians
	float degAngle = glm::clamp(event.GetPolarAngle(), 0.001f, 179.999f);
	m_OrbitAzimuthal = glm::radians(event.GetAzimuthalAngle());
	m_OrbitPolar = glm::radians(degAngle);
	
	// Recalculate
	RecalculateCameraPositionFromSphericalCoords();
	// No other events need this, return true.
	return true;
}

bool ViewportLayer::OnAnimationChange(AnimationChangeEvent& event)
{
	// New Animation changed. All Animations loaded onto model when it is loaded, so just play by name.
	m_Animator->PlayAnimation(event.GetAnimationName());

	return true;
}

bool ViewportLayer::OnExport(ExportEvent& event)
{
	ExportAnimationSpriteSheet();
	return true;
}

bool ViewportLayer::OnFPSChanged(AnimationFPSChangeEvent& event)
{
	m_FrameTime = (1 / event.GetFPS());
	return true;
}




