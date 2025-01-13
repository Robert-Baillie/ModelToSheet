#pragma once

#include "Renderer/VertexArray.h"
#include "Input.h"
#include "Renderer/Renderer.h"	
#include "Scene/Camera.h"
#include "ResourceManager.h"
#include <filesystem>
#include <glm/gtc/matrix_transform.hpp>

#include "Renderer/Model.h"
#include "Renderer/Shader.h"
#include "Renderer/Texture.h"
#include "Renderer/Material.h"


class ExampleLayer : public Layer {
public:
    ExampleLayer() : Layer("Example"),
        m_Camera(45.0f, 1920.0f / 1080.0f, 0.1f, 100.0f)  // FOV, aspect ratio, near, far
    {
       
        m_Shader = RESOURCE_MANAGER.GetDefaultShader();

       
        std::string executablePath = std::filesystem::current_path().string();
        std::string modelPath = executablePath + "/Assets/Models/scene.gltf";

        // Load model through resource manager
        m_Model = RESOURCE_MANAGER.LoadModel(modelPath, "Dog");
        if (!m_Model) {
            ERROR_LOG("Failed to load model: {0}", modelPath);
            return;
        }

        // To do: sort this shit below into their corresponding classes.
        // Create default material properties
        MaterialProperties defaultProps;
        defaultProps.ambient = 0.5f;
        defaultProps.diffuse = 0.8f;
        defaultProps.specular = 0.01f;
        defaultProps.shininess = 32.0f;
        defaultProps.shader = m_Shader;

        // Since we can't modify the material list directly, we need to work with temporary materials
        // and create a new model if needed
        const auto& materials = m_Model->GetMaterialList();
        bool needsDefaultMaterials = materials.empty();

        std::vector<std::shared_ptr<Material>> updatedMaterials;
        if (!needsDefaultMaterials) {
            // Copy existing materials and update them
            for (const auto& material : materials) {
                if (material) {
                    // Create new material with existing properties
                    auto props = material->GetProperties();
                    props.shader = m_Shader;

                    // If no maps, use defaults
                    if (!props.diffuseMap) {
                        props.diffuseMap = RESOURCE_MANAGER.GetDefaultTexture();
                    }
                    if (!props.specularMap) {
                        props.specularMap = RESOURCE_MANAGER.GetDefaultTexture();
                    }

                    // Create new material with updated properties
                    auto updatedMaterial = RESOURCE_MANAGER.CreateMaterial(props, "UpdatedMaterial_" + std::to_string(updatedMaterials.size()));
                    updatedMaterials.push_back(updatedMaterial);
                }
                else {
                    // For null materials, add default
                    updatedMaterials.push_back(RESOURCE_MANAGER.GetDefaultMaterial());
                }
            }
        }
        else {
            // If no materials, create one with defaults
            auto defaultMaterial = RESOURCE_MANAGER.CreateMaterial(defaultProps, "DefaultModelMaterial");
            updatedMaterials.push_back(defaultMaterial);
        }

        // Create a new model with the updated materials if needed
        if (needsDefaultMaterials || !materials.empty()) {
            m_Model->SetMaterials(updatedMaterials);
        }
        
    }


    void OnUpdate() override {
        ControlCamera();

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
        if (m_Shader) {
            m_Shader->Bind();
            m_Shader->UploadUniformMat4("u_ViewProjection", m_Camera.GetViewProjectionMatrix());
            m_Shader->UploadUniformMat4("u_Transform", transform);
            m_Shader->UploadUniformFloat3("lightPos", { 1.0f, 1.0f, 1.0f });
            m_Shader->UploadUniformFloat3("viewPos", m_CameraPosition);
        }

        // Draw the model if it exists
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
    float m_DeltaTime = 0.016f;

    void ControlCamera() {
        glm::vec3 position = m_Camera.GetPosition();

        if (Input::IsKeyPressed(KEY_A))
            position.x -= m_CameraSpeed * m_DeltaTime;
        if (Input::IsKeyPressed(KEY_D))
            position.x += m_CameraSpeed * m_DeltaTime;
        if (Input::IsKeyPressed(KEY_W))
            position.z -= m_CameraSpeed * m_DeltaTime;
        if (Input::IsKeyPressed(KEY_S))
            position.z += m_CameraSpeed * m_DeltaTime;
        if (Input::IsKeyPressed(KEY_E))
            position.y += m_CameraSpeed * m_DeltaTime;
        if (Input::IsKeyPressed(KEY_Q))
            position.y -= m_CameraSpeed * m_DeltaTime;

        m_Camera.SetPosition(position);
        m_CameraPosition = position;

        float rotation = m_Camera.GetRotation();
        if (Input::IsKeyPressed(KEY_LEFT))
            rotation += m_RotationSpeed * m_DeltaTime;
        if (Input::IsKeyPressed(KEY_RIGHT))
            rotation -= m_RotationSpeed * m_DeltaTime;

        m_Camera.SetRotation(rotation);
    }
};