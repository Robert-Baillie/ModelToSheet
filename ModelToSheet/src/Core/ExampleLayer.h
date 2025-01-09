#pragma once

#include "Renderer/VertexArray.h"
#include "Input.h"
#include "Renderer/Renderer.h"	
#include "Scene/Camera.h"
#include "Renderer/Texture.h"
#include "Renderer/Material.h"
#include "Renderer/Model.h"

#include <filesystem>
#include <glm/gtc/matrix_transform.hpp> // temp

class ExampleLayer : public Layer {
public:
    ExampleLayer() : Layer("Example"),
        m_Camera(45.0f, 1920.0f / 1080.0f, 0.1f, 100.0f)  // FOV, aspect ratio, near, far
    {   
        std::filesystem::path currentPath = std::filesystem::path(__FILE__).parent_path();

        // Find Project Root and the file location
        std::filesystem::path vertexPath = currentPath.parent_path().parent_path() /  "resources" / "Shaders" / "Vertex.shader";
        std::filesystem::path fragmentPath = currentPath.parent_path().parent_path() / "resources" / "Shaders" / "Fragment.shader";


        // Create a shader with support for material properties
        m_Shader = Shader::CreateFromFile(vertexPath.string(), fragmentPath.string(), "ExampleShader");
        
        
        
        try {
            std::string executablePath = std::filesystem::current_path().string();
            // Load the model with its name
             std::string modelPath = executablePath + "/Assets/Models/IronMan.obj";
             m_Model = std::make_shared<Model>(modelPath, "IronMan");
            // std::string modelPath = executablePath + "/Assets/Models/scene.gltf";
            // m_Model = std::make_shared<Model>(modelPath, "Dog");

            // Create default material properties for the shader
            MaterialProperties defaultProps;
            defaultProps.ambient = 0.5f;
            defaultProps.diffuse = 0.8f;
            defaultProps.specular = 0.01f;
            defaultProps.shininess = 32.0f;
            defaultProps.shader = m_Shader;

            // Create default texture if needed 
            if (!m_Model->GetMaterialList().empty() && !m_Model->GetMaterialList()[0]->GetProperties().diffuseMap) {
                Texture::Specification texSpec = Texture::GetDefaultSpecification();

                // Create a simple checkerboard texture
                unsigned char textureData[16 * 16 * 4];
                for (int y = 0; y < 16; y++) {
                    for (int x = 0; x < 16; x++) {
                        int index = (y * 16 + x) * 4;
                        bool isBlack = (x + y) % 2 == 0;
                        textureData[index] = isBlack ? 0 : 255;
                        textureData[index + 1] = isBlack ? 0 : 255;
                        textureData[index + 2] = isBlack ? 0 : 255;
                        textureData[index + 3] = 255;
                    }
                }

                auto defaultTexture = std::shared_ptr<Texture>(
                    Texture::Create(16, 16, texSpec, textureData));
                defaultProps.diffuseMap = defaultTexture;
                defaultProps.specularMap = defaultTexture; 
            }

            // Update all materials with the shader. Should this not be in the model itself?
            for (auto& material : m_Model->GetMaterialList()) {
                if (material) {
                    auto props = material->GetProperties();
                    props.shader = m_Shader;
                    material->SetProperties(props);
                }
            }
        }
        catch (const std::exception& e) {
            ERROR_LOG("Failed to load model: {0}", e.what());
        }
    }

    void OnUpdate() override {
        ControlCamera(); // Should be split into collect inputs and update camera after render 

        // Update model rotation
        m_ModelRotation += 0.5f;
        if (m_ModelRotation >= 360.0f) {
            m_ModelRotation -= 360.0f;
        }

        float angle = m_ModelRotation * 3.14159f / 180.0f;
        glm::mat4 transform = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f));

        // Rendering
        RenderCommand::SetClearColour({ 0.1f, 0.1f, 0.1f, 1 });
        RenderCommand::Clear();

        Renderer::BeginScene(m_Camera);

        // Update shader uniforms
        m_Shader->Bind();
        m_Shader->UploadUniformMat4("u_ViewProjection", m_Camera.GetViewProjectionMatrix());
        m_Shader->UploadUniformMat4("u_Transform", transform);

        // Upload light position and view position. Set Light as constant and set View Pos to the camera
        m_Shader->UploadUniformFloat3("lightPos",{1.0f, 1.0f, 1.0f});
        m_Shader->UploadUniformFloat3("viewPos", m_CameraPosition);

        // Draw the model - it will handle its own materials
        if (m_Model) {
            m_Model->Draw();
        }

        Renderer::EndScene();
    }

private:
    std::shared_ptr<Shader> m_Shader;
    std::shared_ptr<Model> m_Model;

    PerspectiveCamera m_Camera;
    glm::vec3 m_CameraPosition = { 0.0f, 2.0f, 1.0f };
    float m_CameraSpeed = 15.0f;
    float m_ModelRotation = 0.0f;
    float m_RotationSpeed = 20.0f;
    float m_DeltaTime = 0.016f; // Simulating 60fps


private:
    void ControlCamera() {
        // Position control
        glm::vec3 position = m_Camera.GetPosition();

        if (Input::IsKeyPressed(KEY_A))  // Left
            position.x -= m_CameraSpeed * m_DeltaTime;
        if (Input::IsKeyPressed(KEY_D))  // Right
            position.x += m_CameraSpeed * m_DeltaTime;
        if (Input::IsKeyPressed(KEY_W))  // Forward
            position.z -= m_CameraSpeed * m_DeltaTime;
        if (Input::IsKeyPressed(KEY_S))  // Backward
            position.z += m_CameraSpeed * m_DeltaTime;
        if (Input::IsKeyPressed(KEY_E))  // Up
            position.y += m_CameraSpeed * m_DeltaTime;
        if (Input::IsKeyPressed(KEY_Q))  // Down
            position.y -= m_CameraSpeed * m_DeltaTime;

        m_Camera.SetPosition(position);

        m_CameraPosition = position;

        // Rotation control
        float rotation = m_Camera.GetRotation();
        if (Input::IsKeyPressed(KEY_LEFT))  // Rotate counterclockwise
            rotation += m_RotationSpeed * m_DeltaTime;
        if (Input::IsKeyPressed(KEY_RIGHT)) // Rotate clockwise
            rotation -= m_RotationSpeed * m_DeltaTime;

        m_Camera.SetRotation(rotation);

    }

   
};