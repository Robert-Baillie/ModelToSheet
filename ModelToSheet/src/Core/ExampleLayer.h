#pragma once

#include "Renderer/VertexArray.h"
#include "Input.h"
#include "Renderer/Renderer.h"	
#include "Renderer/Camera.h"
#include "Renderer/Texture.h"
#include "Renderer/Material.h"

class ExampleLayer : public Layer {
public:
    ExampleLayer() : Layer("Example"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f) {
        // Vertex array setup remains the same
        m_VertexArray.reset(VertexArray::Create());

        float texturedQuadVertices[5 * 4] = {
            -0.5f, -0.5f, 0.0f,   0.0f, 0.0f,
             0.5f, -0.5f, 0.0f,   1.0f, 0.0f,
             0.5f,  0.5f, 0.0f,   1.0f, 1.0f,
            -0.5f,  0.5f, 0.0f,   0.0f, 1.0f
        };

        std::shared_ptr<VertexBuffer> vertexBuffer;
        vertexBuffer.reset(VertexBuffer::Create(texturedQuadVertices, sizeof(texturedQuadVertices)));
        vertexBuffer->SetLayout({
            { ShaderDataType::Float3, "a_Position"},
            { ShaderDataType::Float2, "a_TexCoord"}
            });
        m_VertexArray->AddVertexBuffer(vertexBuffer);

        unsigned int quadIndices[6] = { 0, 1, 2, 2, 3, 0 };
        std::shared_ptr<IndexBuffer> indexBuffer;
        indexBuffer.reset(IndexBuffer::Create(quadIndices, sizeof(quadIndices) / sizeof(uint32_t)));
        m_VertexArray->SetIndexBuffer(indexBuffer);

        // Updated shader with material properties
        std::string vertexSrc = R"(
            #version 330 core
            layout(location = 0) in vec3 a_Position;
            layout(location = 1) in vec2 a_TexCoord;
            
            uniform mat4 u_ViewProjection;
            out vec2 v_TexCoord;

            void main() {
                v_TexCoord = a_TexCoord;
                gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
            }
        )";

        std::string fragmentSrc = R"(
            #version 330 core
            layout(location = 0) out vec4 color;
            
            in vec2 v_TexCoord;
            
            uniform sampler2D u_DiffuseMap;
            uniform float u_Specular;
            uniform float u_Shininess;
            
            void main() {
                vec4 texColor = texture(u_DiffuseMap, v_TexCoord);
                color = texColor * vec4(1.0 + u_Specular, 1.0 + u_Specular, 1.0 + u_Specular, 1.0);
            }
        )";

        m_Shader.reset(new Shader(vertexSrc, fragmentSrc));

        // Create texture
        Texture::Specification texSpec;
        texSpec.internalFormat = TextureFormat::RGBA;
        texSpec.imageFormat = TextureFormat::RGBA;

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

        m_Texture.reset(Texture::Create(16, 16, texSpec, textureData));

        // Create material
        MaterialProperties matProps;
        matProps.ambient = 0.2f;
        matProps.diffuse = 0.8f;
        matProps.specular = 0.5f;
        matProps.shininess = 32.0f;
        matProps.diffuseMap = m_Texture.get();

        m_Material.reset(Material::Create(matProps));
    }

    void OnUpdate() override {
        // Camera controls remain the same
        if (Input::IsKeyPressed(KEY_LEFT))
            m_CameraPosition.x -= m_CameraSpeed;
        if (Input::IsKeyPressed(KEY_RIGHT))
            m_CameraPosition.x += m_CameraSpeed;
        if (Input::IsKeyPressed(KEY_UP))
            m_CameraPosition.y += m_CameraSpeed;
        if (Input::IsKeyPressed(KEY_DOWN))
            m_CameraPosition.y -= m_CameraSpeed;

        RenderCommand::SetClearColour({ 0.1f, 0.1f, 0.1f, 1 });
        RenderCommand::Clear();

        m_Camera.SetPosition(m_CameraPosition);
        Renderer::BeginScene(m_Camera);

        // Get uniform locations
        uint32_t specularLoc = m_Shader->GetUniformLocation("u_Specular");
        uint32_t shininessLoc = m_Shader->GetUniformLocation("u_Shininess");


        // Bind shader and use material
        m_Shader->Bind();
        m_Material->UseMaterial(specularLoc, shininessLoc);

        Renderer::Submit(m_Shader, m_VertexArray);
        Renderer::EndScene();
    }

private:
    std::shared_ptr<Shader> m_Shader;
    std::shared_ptr<VertexArray> m_VertexArray;
    std::shared_ptr<Texture> m_Texture;
    std::shared_ptr<Material> m_Material;

    OrthographicCamera m_Camera;
    glm::vec3 m_CameraPosition = { 0.0f, 0.0f, 0.0f };
    float m_CameraSpeed = 0.1f;
};