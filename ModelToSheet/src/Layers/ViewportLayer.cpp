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
	 dispatcher.Dispatch<AnimationKeyChangeEvent>(BIND_EVENT_FN(ViewportLayer::OnKeyFrameChanged));
	 dispatcher.Dispatch<KeyframeChangeEvent>(BIND_EVENT_FN(ViewportLayer::OnKeyFrameSliderValueChanged));
	 dispatcher.Dispatch<TextureSizeChangeEvent>(BIND_EVENT_FN(ViewportLayer::OnTextureSizeChange));
}

void ViewportLayer::OnUpdate()
{
	// Handle any preview mode
	if (m_KeyframePreviewMode && !m_KeyFrameIndexList.empty()) {
		m_AccumulatedPreviewTime += ImGui::GetIO().DeltaTime;
		auto animation = m_Animator->GetCurrentAnimation();

		
		float keyframeDisplayTime = 1.0f / m_PreviewSamplesPerSecond;

		if (m_AccumulatedPreviewTime >= keyframeDisplayTime) {
			m_AccumulatedPreviewTime = 0.0f;
			m_CurrentPreviewFrame = (m_CurrentPreviewFrame + 1) % m_KeyFrameIndexList.size();

			// Set the exact keyframe time and update without delta time
			float keyFrameTime = animation->GetKeyframeTime(m_KeyFrameIndexList[m_CurrentPreviewFrame]);
			m_Animator->SetCurrentTime(keyFrameTime);
			m_Animator->UpdateAnimation(0.0f);  // Using 0.0f prevents further animation
		}
	}
	

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
	int scale = 512 / m_TextureWidth;
	ImVec2 textureSize(m_TextureWidth * scale, m_TextureHeight * scale);
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

		// Quick bug fix, if it wasn't playhing and we switched, the last render on the shader would be used. Update it if it is paused.
		if (!m_IsPlaying) {
			m_IsPlaying = true;
			m_Animator->UpdateAnimation(0.0f);
			RenderScene();
			m_IsPlaying = false;
		}
		

	}

	ImGui::SameLine();
	if (ImGui::Button("Normal")) {
		m_CurrentShader = m_FragmentShaders[FragmentShaderType::Normal];
		m_CurrentFragmentShaderType = FragmentShaderType::Normal;

		// Quick bug fix, if it wasn't playhing and we switched, the last render on the shader would be used. Update it if it is paused.
		if (!m_IsPlaying) {
			m_IsPlaying = true;
			m_Animator->UpdateAnimation(0.0f);
			RenderScene();
			m_IsPlaying = false;
		}
	}



	/* Pause/Play button */
	ImVec2 buttonSize(100, 30);
	float previewPlayWidth = 210.0f;
	float yPadding = 40.0f; // Padding from bottom of window

	// Center position for the controls group
	ImGui::SetCursorScreenPos(ImVec2(
		ImGui::GetWindowPos().x + (sizeIm.x - previewPlayWidth) * 0.5f,
		ImGui::GetWindowPos().y + sizeIm.y - yPadding
	));

	// Button one
	if (ImGui::Button(m_IsPlaying ? "Pause" : "Play", buttonSize)) {
		m_IsPlaying = !m_IsPlaying;
		m_KeyframePreviewMode = false; // Exit preview mode when toggling back
	}
	ImGui::SameLine(0, 10.0f); // Slight right padding
	if (ImGui::Button("Preview", buttonSize)) {
		m_KeyframePreviewMode = true;
		m_IsPlaying = false;
	}

	// Set position of the speed control below these positions
	ImGui::SetCursorScreenPos(ImVec2(
		ImGui::GetWindowPos().x + (sizeIm.x - previewPlayWidth) * 0.5f,
		ImGui::GetWindowPos().y + sizeIm.y - yPadding + 35.0f
	));
	float displayTime = 1.0f / m_PreviewSamplesPerSecond;
	ImGui::PushItemWidth(previewPlayWidth);
	ImGui::SliderInt("Preview Speed", &m_PreviewSamplesPerSecond, 1, 60, "%d samples/sec");
	ImGui::PopItemWidth();

	/* Show the export warning */
	if (m_ShowExportWarning) ShowExportWarning();
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
		if (m_Animator  && (m_IsPlaying || m_KeyframePreviewMode)) {
			// If playing we want to simply update by the frame time
			if (m_IsPlaying) m_Animator->UpdateAnimation(m_FrameTime);

			// if playing and preview we still need to update both transform and shader.
			// In preview mode the time has already been updated for us
			auto transform = m_Animator->GetFinalBoneMatrices();
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
	if (!m_Animator || !m_Animator->GetCurrentAnimation()) return;

	// Get animations, key count and original types so we can revert back to them after exporting
	auto animation = m_Animator->GetCurrentAnimation();
	bool isPlaying = m_IsPlaying;
	int keyframeCount = m_KeyFrameIndexList.size();
	float originalTime = m_Animator->GetCurrentTime(); // Overriding a define here?
	FragmentShaderType originalType = m_CurrentFragmentShaderType;
	m_IsPlaying = true;

	std::vector<FragmentShaderType> shaderTypes = {
		FragmentShaderType::Diffuse,
		FragmentShaderType::Normal
	};



	// Loop over these shader types and export into their own screenshots.
	for (auto shaderType : shaderTypes) {
		// ASsign shader and type for rendering
		m_CurrentShader = m_FragmentShaders[shaderType];
		m_CurrentFragmentShaderType = shaderType;

		// Initialise the bounds
		int minX = m_TextureWidth;
		int minY = m_TextureHeight;
		int maxX = 0;
		int maxY = 0;

		// Store the frame data and the collected bounds for future use
		std::vector<std::vector<unsigned char>> frameDataList;

		// Passing over the frames and detrmining the bounds of their data
		for (int i = 0; i < keyframeCount; i++) {
			// Get the time of keyframe and bind the rendered scene to the framebuffer
			float keyFrameTime = animation->GetKeyframeTime(m_KeyFrameIndexList[i]);
			m_Animator->SetCurrentTime(keyFrameTime);
			m_Framebuffer->Bind();
			RenderScene();

			// Grab the data for this texture
			std::vector<unsigned char> framePixels(m_TextureWidth * m_TextureHeight * 4);
			RenderCommand::ReadPixels(m_TextureWidth, m_TextureHeight, framePixels);
			m_Framebuffer->Unbind();

			// Store the frame data into the main list for the second pass
			frameDataList.push_back(framePixels);

			// Loop over this frame and find the bounds for it
			for (int y = 0; y < m_TextureHeight; y++) {
				for (int x = 0; x < m_TextureWidth; x++) {
					// Get the index of this pixel (flipping)
					int index = ((m_TextureHeight - 1 - y) * m_TextureWidth + x) * 4;


					// Check if the pixel has any content (including alpha)
					if (framePixels[index + 3] > 0) { // Alpha
						minX = std::min(minX, x);
						minY = std::min(minY, y);
						maxX = std::max(maxX, x);
						maxY = std::max(maxY, y);
					}
				}
			}
		}


		// We have the bounds, we want to pad them slightly so we don't have override on the images
		const int padding = 3;
		minX = std::max(0, minX - padding);
		minY = std::max(0, minY - padding);
		maxX = std::min(m_TextureWidth - 1, maxX + padding);
		maxY = std::min(m_TextureHeight - 1, maxY + padding);

		// New dimensions of the width and height
		int newWidth = maxX - minX + 1;
		int newHeight = maxY - minY + 1;

		// Create trimmed sprite sheet
		std::vector<unsigned char> spriteSheetPixels(
			newWidth * newHeight * 4 * keyframeCount, 0 //Initialise with 0
		);

		// Second pass: copy only the content area to sprite sheet
		for (int i = 0; i < keyframeCount; i++) {
			const auto& framePixels = frameDataList[i];

			for (int y = minY; y <= maxY; y++) {
				size_t destOffset = ((y - minY) * newWidth * keyframeCount + i * newWidth) * 4;
				size_t srcOffset = ((m_TextureHeight - 1 - y) * m_TextureWidth + minX) * 4;

				memcpy(
					spriteSheetPixels.data() + destOffset,
					framePixels.data() + srcOffset,
					newWidth * 4
				);
			}
		}

		// Save the trimmed sprite sheet
		std::string filename = fmt::format(
			"{}_keyframe_spritesheet.png",
			Shader::GetShaderTypeName(shaderType)
		);
		std::string executablePath = std::filesystem::current_path().string();
		std::string fullPath = executablePath + "/Screenshots/" + filename;

		std::filesystem::create_directories(
			std::filesystem::path(fullPath).parent_path()
		);

		stbi_write_png(
			fullPath.c_str(),
			newWidth * keyframeCount,
			newHeight,
			4,
			spriteSheetPixels.data(),
			newWidth * 4 * keyframeCount
		);


	}

	// Restore original state
	m_Animator->SetCurrentTime(originalTime);
	m_IsPlaying = isPlaying;
	m_CurrentShader = m_FragmentShaders[originalType];
	m_CurrentFragmentShaderType = originalType;
}

void ViewportLayer::ShowExportWarning()
{
	ImGui::OpenPopup("Export Warning");

	if (ImGui::BeginPopupModal("Export Warning", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text("No frames selected! Please select at least one frame before exporting.");
		if (ImGui::Button("OK")) {
			m_ShowExportWarning = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}


/* Event Methods */
bool ViewportLayer::OnModelLoadStart(ModelLoadStartEvent& event)
{
	m_IsModelLoading = true;
	
	// A model has been clicked in the UI. Load it as intended
	LoadModel(event.GetPath(), event.GetModelName());
	
	m_IsModelLoading = false;
	m_IsPlaying = true;
	m_KeyFrameIndexList.clear();
	m_KeyframePreviewMode = false;
	
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
	m_IsPlaying = true;
	m_KeyFrameIndexList.clear();
	m_KeyframePreviewMode = false;
	return true;
}

bool ViewportLayer::OnExport(ExportEvent& event)
{
	if (m_KeyFrameIndexList.empty()) {
		m_ShowExportWarning = true;
		return true;
	}

	ExportAnimationSpriteSheet();
	return true;
}

bool ViewportLayer::OnKeyFrameChanged(AnimationKeyChangeEvent& event)
{
	// Copy straight into this index list
	m_KeyFrameIndexList = event.GetIndexList();

	// nothing else needs this
	return true;
}

bool ViewportLayer::OnKeyFrameSliderValueChanged(KeyframeChangeEvent& event)
{
	// Switch to true to make sure it renders
	m_IsPlaying = true;

	// Get the animation time and start playing
	auto time = m_Animator->GetCurrentAnimation()->GetKeyframeTime(event.GetIndex());
	m_Animator->SetCurrentTime(time);
	m_Animator->UpdateAnimation(0.0f);
	RenderScene();

	// Switch to false playing so it stays at the frame.
	m_IsPlaying = false;
	m_KeyframePreviewMode = false;
	return true;
}

bool ViewportLayer::OnTextureSizeChange(TextureSizeChangeEvent& event)
{
	// Reassign
	m_TextureWidth = event.GetTextureSize();
	m_TextureHeight = event.GetTextureSize();
	m_Framebuffer->Resize(m_TextureHeight, m_TextureHeight);

	return true;
}




